#include "WProgram.h"
#include "wltactileutil.h"

#include "costable_12bit.h"

//#define NORMAL_MODE
#define LATERAL_MODE

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

// Signal configuration
const unsigned int samplingRate = 25000; //16129
const unsigned int sinTableResolution = 12;

// Memory configuration
const byte outputCount = 4;
const byte frequenciesPerOutput = 10;

struct actuatorConfiguration
{
  uint16_t frequency_q5[frequenciesPerOutput]; //   0 .. 1024 + 5 bit fractional
  uint16_t amplitude_q15[frequenciesPerOutput]; // -1.0 .. 1.0
};

volatile actuatorConfiguration* volatile actuators;
const q15_t amplitudeClip = float2q15(0.95);
q15_t actuatorPhase[outputCount][frequenciesPerOutput];

inline q15_t frequency2increment(q5_t freq)
{
    return (q15_t) ((q31_t(freq) << 15) / (q31_t(samplingRate) << 5));
}

q15_t amplitudeAverage[outputCount][frequenciesPerOutput] = {{float2q15(0.5)}};
q5_t frequencyAverage[outputCount][frequenciesPerOutput] = {{float2q5(0.0)}};

inline q15_t ema_15(q15_t in, q15_t avg)
{
    // alpha = 0.03125: Approx 84.23Hz -3dB Cutoff frequency (for 16kHz sampling rate)
    return avg - (avg >> 5) + (in >> 5);

    // alpha = 0.015625: Approx 62.66Hz -3dB Cutoff frequency (for 25kHz sampling rate)
    //return avg - (avg >> 6) + (in >> 6);
}


void step()
{
  q15_t phases[outputCount] = {0};

  for(int n = 0; n < outputCount; n++)
  {
    for(int m = 0; m < frequenciesPerOutput; m++)
    {
        amplitudeAverage[n][m] = ema_15(actuators[n].amplitude_q15[m], amplitudeAverage[n][m]);
        frequencyAverage[n][m] = ema_15(actuators[n].frequency_q5[m], frequencyAverage[n][m]);

        q15_t amplitude = amplitudeAverage[n][m];
        q5_t frequency = frequencyAverage[n][m];

        q15_t phase = actuatorPhase[n][m];

        phase += frequency2increment(frequency);

        // Wrap to q15 range (0.0 .. 1.0) = (0.0 .. 2pi)
        if(phase < 0) phase += 32768;

        // Prevent steps
        if(amplitude == 0) phase = float2q15(1.0/4.0); //float2q15(M_PI/2.0) => 0.0;

        // For quasi-static signal parts
        if(frequency == 0) phase = 0;

        // Calculate sum(sin(phase[m]))
        unsigned int table_index = phase >> (15 - sinTableResolution);
        q15_t cos_result = cos_table[table_index];

#ifdef LATERAL_MODE
        phases[n] += mulQ15(cos_result, amplitude);
#endif

#ifdef NORMAL_MODE
        const q15_t amplitudehalf = amplitude >> 1;
        phases[n] += mulQ15(cos_result, amplitudehalf) + amplitudehalf;
#endif
        actuatorPhase[n][m] = phase;
    }
  }

  // Scale the -1.0 ... 1.0 range to -0.9375...0.9375 in order to reach the desired -240...240 output for pwm
//  arm_scale_q15(&phases[0], amplitudeScale, 0, &phases[0], outputCount);

#ifdef LATERAL_MODE
  //for(int n = 0; n < 4; n++)
  //    if(phases[n] > amplitudeClip)
  //        phases[n] = amplitudeClip;

  wlTactileUtil::instance().analogOutQ15(&phases[0]);
#endif

#ifdef NORMAL_MODE
    const int shift = 32;
    const int shiftPrec = 6;
    arm_scale_q15(&phases[0], float2q15(0.9), 0, &phases[0], outputCount);


    wlTactileUtil::instance().analogOutFast<0, 9>((int(phases[0]) >> shiftPrec) + shift);
    wlTactileUtil::instance().analogOutFast<1, 9>((int(phases[1]) >> shiftPrec) + shift);
    wlTactileUtil::instance().analogOutFast<2, 9>((int(phases[2]) >> shiftPrec) + shift);
    wlTactileUtil::instance().analogOutFast<3, 9>((int(phases[3]) >> shiftPrec) + shift);
#endif
}

extern "C" int main(void)
{
    actuators = reinterpret_cast<volatile actuatorConfiguration*>(wlTactileUtil::instance().requestDMABuffer(sizeof(actuatorConfiguration)*outputCount));

    // Initialize memory
    for(int n = 0; n < outputCount; n++)
    {
      for(int m = 0; m < frequenciesPerOutput; m++)
      {
        actuators[n].frequency_q5[m] = 0;
        actuators[n].amplitude_q15[m] = 0;
      }
    }

    wlTactileUtil::instance().init(step);    
}


