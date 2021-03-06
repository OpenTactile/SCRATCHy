#include "WProgram.h"
#include "wltactileutil.h"

#include "costable_12bit.h"

typedef q15_t q5_t;

#define float2q15(x) (q15_t(x*32768))
#define float2q5(x) (q15_t(x*32))

inline q15_t mulQ15(q15_t a, q15_t b)
{
    return q15_t((int32_t(a) * int32_t(b)) >> 15);
}

inline q15_t mulQ15(q15_t a, q31_t b)
{
    return q15_t((int32_t(a) * b) >> 15);
}

// Sampling rate
const unsigned int samplingRate = 25000; //16129

// Size of the cosine table specified in the include section
const unsigned int sinTableResolution = 12;

// Memory configuration
const byte outputCount = 4;
const byte frequenciesPerOutput = 10;

struct actuatorConfiguration
{
  uint16_t frequency_q5[frequenciesPerOutput];  //    0 .. 1024 + 5 bit fractional
  uint16_t amplitude_q15[frequenciesPerOutput]; // -1.0 .. 1.0
};

// The "Frequency Table" that will be mapped to DMA
volatile actuatorConfiguration* volatile actuators;

// Phase accumulation buffer
q15_t actuatorPhase[outputCount][frequenciesPerOutput];
inline q15_t frequency2increment(q5_t freq)
{
    return (q15_t) ((q31_t(freq) << 15) / (q31_t(samplingRate) << 5));
}

// 1st order digital IIR filter for smoothing
// amplitudes and frequencies over time
q15_t amplitudeAverage[outputCount][frequenciesPerOutput] = {{float2q15(0.5)}};
q5_t frequencyAverage[outputCount][frequenciesPerOutput] = {{float2q5(0.0)}};
inline q15_t ema_15(q15_t in, q15_t avg)
{
    // alpha = 0.03125: Approx 84.23Hz -3dB Cutoff frequency (for 16kHz sampling rate)
    //return avg - (avg >> 5) + (in >> 5);

    // alpha = 0.015625: Approx 62.66Hz -3dB Cutoff frequency (for 25kHz sampling rate)
    return avg - (avg >> 6) + (in >> 6);
}


void step()
{
  q15_t phases[outputCount] = {0};

  // Create signal by superposition of cosine waveforms
  // Continuity of signal is ensured by using phase accumulation.
  for(int n = 0; n < outputCount; n++)
  {
    for(int m = 0; m < frequenciesPerOutput; m++)
    {
        // Smoothen changes in amplitude and frequency
        // using a 1st order IIR filter
        amplitudeAverage[n][m] = ema_15(actuators[n].amplitude_q15[m],
                                        amplitudeAverage[n][m]);
        frequencyAverage[n][m] = ema_15(actuators[n].frequency_q5[m],
                                        frequencyAverage[n][m]);

        q15_t amplitude = amplitudeAverage[n][m];
        q5_t frequency  = frequencyAverage[n][m];

        q15_t phase = actuatorPhase[n][m];

        phase += frequency2increment(frequency);

        // Wrap to q15 range (0.0 .. 1.0) = (0.0 .. 2pi)
        if(phase < 0) phase += 32768;

        // Prevent steps
        if(amplitude == 0) phase = float2q15(1.0/4.0); //float2q15(M_PI/2.0) => 0.0;

        // If the frequency is set to zero, the signal form can be controlled
        // by the MCU directly (modulation can be done by varying the amplitude)
        if(frequency == 0) phase = 0;

        // Lookup cosine table
        // Quality could be increased by linear interpolation using the
        // fractional part. This, however, would also need more time resulting
        // in lower samplingrate.
        unsigned int table_index = phase >> (15 - sinTableResolution);
        q15_t cos_result = cos_table[table_index];

        phases[n] += mulQ15(cos_result, amplitude);

        actuatorPhase[n][m] = phase;
    }
  }

  // Depending on the type of signal board, the maximum amplitude may be limited
  // to a PWM value smaller than 1.0 (e. g. because of bootstrap capacitors of
  // the HVA). This problem can be solved either on the MCU (software clipping)
  // or by uncommenting one of the following blocks:

  // Scale maximum PWM value
  // const q15_t amplitudeScale = float2q15(0.95);
  // arm_scale_q15(&phases[0], amplitudeScale, 0, &phases[0], outputCount);

  // Clip maximum PWM value
  // const q15_t amplitudeClip = float2q15(0.95);
  // for(int n = 0; n < 4; n++)
  //    if(phases[n] > amplitudeClip)
  //        phases[n] = amplitudeClip;

  wlTactileUtil::instance().analogOutQ15(&phases[0]);
}

extern "C" int main(void)
{
    // Allocate memory for Frequency Tables that is automatically synchronized
    actuators = reinterpret_cast<volatile actuatorConfiguration*>(
                wlTactileUtil::instance().requestDMABuffer(
                    sizeof(actuatorConfiguration)*outputCount
                    )
                );

    // Initialize memory
    for(int n = 0; n < outputCount; n++)
    {
      for(int m = 0; m < frequenciesPerOutput; m++)
      {
        actuators[n].frequency_q5[m] = 0;
        actuators[n].amplitude_q15[m] = 0;
      }
    }

    // Initialize SignalGenerator, the 'step' function will be called
    // within Runlevel 3
    wlTactileUtil::instance().init(step);    
}


