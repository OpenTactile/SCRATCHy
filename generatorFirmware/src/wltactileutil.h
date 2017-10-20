#ifndef WLTACTILEUTIL_H
#define WLTACTILEUTIL_H


#include "WProgram.h"
#include "DMAChannel.h"


#include <i2c_t3.h>
#include <core_pins.h>

#define ARM_MATH_CM4
#include <arm_math.h>

class wlTactileUtil
{
public:
    static wlTactileUtil& instance()
    {
        static wlTactileUtil _instance;
        return _instance;
    }

    ~wlTactileUtil(){}

    enum class SystemLED
    {
        Alive     = 0,
        Sync      = 1,
        Data      = 2,
        Interrupt = 3
    };

    enum class SystemStatus
    {
        Operational    = 0b0000,
        ThermalWarning = 0b0010,
        NotInSync      = 0b0100,
        ConfigInvalid  = 0b1000
    };

    enum SystemRequest
    {
        // Can be received at any time
        Reset               = 0b11111111, // Performs a soft-reset using _reboot_Teensyduino_()

        // Can be received at any time
        Alive               = 0b00000000,
        Status              = 0b00000001,
        StatusReset         = 0b00000011,

        // Runlevel 0:
        // The Signalboard just has been booted, the internal I2C address
        // is set to 0b0000000 till the mainboard sets a new one.
        // A new address is only set if SCK is low after 'SetI2CAddress'
        // has been sent.
        FinishR0            = 0b00011111, // Advance to runlevel 1
        SetI2CAddress       = 0b00010000, // 2nd Byte contains the new address

        // Runlevel 1:
        // Performance measurements are taken out to determine the maximum
        // reliable speed for SPI communication. Several packages of 1kByte
        // size are sent by the mainboard via SPI and a checksum (CRC32) is
        // requested. This test can be performed several times.
        // FIXME: This code has not been tested yet
        //        due to problems with resetting SPI communication
        FinishR1            = 0b00101111, // Advance to runlevel 2
        SPIPrepare          = 0b00100001,
        SPIChecksum         = 0b00100000, // Calculate and send back CRC32 checksum

        // Runlevel 2:
        // Configuration of the signalgenerator
        FinishR2            = 0b00111111, // Advance to runlevel 3
        SetSamplingRate     = 0b00110000,
        SetDACResolution    = 0b00110001,
        SetPanicMask        = 0b00110010,

        // Runlevel 3:
        // Run the frequency generator
        FinishR3            = 0b01001111, // Advance to runlevel 4
        StartLoop           = 0b01000000,

        // Runlevel 4:
        // No operations are carried out at runlevel 4.
        // This mode is used after powering down the mainboard
        // Only 'Reset' and Status commands are accepted.

        // Runlevel R (5):
        // Recovery state: Whenever the SPI transfer is out of sync, the Teensy
        // switches to "recovery mode" allowing to resync with the mainboard
        FinishRecovery      = 0b01011111,

        // Runlevel P (6):
        // Panic: A fatal error has occured, no further operation is performed
        // till the module has been resetted

        // Runlevel -1:
        // Wait for wakeupcall from main board
        InitSystem          = 0b10101010
    };

    void Init_Idle();
    void R0_Idle();

    void R1_PrepareDMATest();
    void R1_Idle();

    void R2_SetDefaults();
    void R2_SetSamplingRate(unsigned int samplingRate);
    void R2_SetDACResolution(unsigned char dacBits);
    void R2_SetPanicMask(unsigned char mask);
    void R2_Idle();

    void R3_StartMainLoop();
    void R3_StopMainLoop();

    void R4_Idle();
    void Rp_Idle();

    unsigned long checkCRC32();

    volatile unsigned char * volatile  requestDMABuffer(unsigned int bufferSize);

    void init(void(*loop)());

    void initSPI2DMA(volatile unsigned char * volatile dmaMemory, unsigned int dmaMemoryLength);
    void initI2C(unsigned int address);


    inline void analogOut(unsigned char output, int value)
    {        
        analogWrite(PWMOut[output], value);
    }

    template<unsigned char OUT>
    inline void analogOutQ15(q15_t valueFixed)
    {
        const uint32_t bits = this->dacBits;
        uint32_t value = (int(valueFixed) >> (15 - (bits - 1))) + ((1 << (bits-1)) - 1);

        const char FTM1_CH0_PIN = 3;
        const char FTM1_CH1_PIN = 4;
        const char FTM0_CH7_PIN = 5;
        const char FTM0_CH4_PIN = 6;

        switch(PWMOut[OUT])
        {
          case FTM0_CH4_PIN: // PTD4, FTM0_CH4
            FTM0_C4V = (value * (uint32_t)(FTM0_MOD + 1)) >> bits;
            // FTM_PINCFG(FTM0_CH4_PIN) = CORE_PIN6_CONFIG
            CORE_PIN6_CONFIG = PORT_PCR_MUX(4) | PORT_PCR_DSE | PORT_PCR_SRE;
            break;
          case FTM0_CH7_PIN: // PTD7, FTM0_CH7
            FTM0_C7V = (value * (uint32_t)(FTM0_MOD + 1)) >> bits;
            // FTM_PINCFG(FTM0_CH7_PIN) =
            CORE_PIN5_CONFIG = PORT_PCR_MUX(4) | PORT_PCR_DSE | PORT_PCR_SRE;
            break;
          case FTM1_CH0_PIN: // PTA12, FTM1_CH0
            FTM1_C0V = (value * (uint32_t)(FTM1_MOD + 1)) >> bits;
            // FTM_PINCFG(FTM1_CH0_PIN)
            CORE_PIN3_CONFIG = PORT_PCR_MUX(3) | PORT_PCR_DSE | PORT_PCR_SRE;
            break;
          case FTM1_CH1_PIN: // PTA13, FTM1_CH1
            FTM1_C1V = (value * (uint32_t)(FTM1_MOD + 1)) >> bits;
            // FTM_PINCFG(FTM1_CH1_PIN)
            CORE_PIN4_CONFIG = PORT_PCR_MUX(3) | PORT_PCR_DSE | PORT_PCR_SRE;
            break;
        }
    }

    inline void analogOutQ15(q15_t* values)
    {
        const uint32_t bits = this->dacBits;
        const uint32_t  offset = ((1 << (bits-1)) - 1);
        const int32_t shift = (15 - (int32_t(bits) - 1));
        const uint32_t value3 = uint32_t((int(values[0]) >> shift) + offset);
        const uint32_t value4 = uint32_t((int(values[1]) >> shift) + offset);
        const uint32_t value5 = uint32_t((int(values[2]) >> shift) + offset);
        const uint32_t value6 = uint32_t((int(values[3]) >> shift) + offset);

        FTM1_C0V = (value3 * (uint32_t)(FTM1_MOD + 1)) >> bits;
        CORE_PIN3_CONFIG = PORT_PCR_MUX(3) | PORT_PCR_DSE | PORT_PCR_SRE;

        FTM1_C1V = (value4 * (uint32_t)(FTM1_MOD + 1)) >> bits;
        CORE_PIN4_CONFIG = PORT_PCR_MUX(3) | PORT_PCR_DSE | PORT_PCR_SRE;

        FTM0_C7V = (value5 * (uint32_t)(FTM0_MOD + 1)) >> bits;
        CORE_PIN5_CONFIG = PORT_PCR_MUX(4) | PORT_PCR_DSE | PORT_PCR_SRE;

        FTM0_C4V = (value6 * (uint32_t)(FTM0_MOD + 1)) >> bits;
        CORE_PIN6_CONFIG = PORT_PCR_MUX(4) | PORT_PCR_DSE | PORT_PCR_SRE;
    }

    template<unsigned char OUT, unsigned char BITS>
    inline void analogOutFast(int value)
    {
        const char FTM1_CH0_PIN = 3;
        const char FTM1_CH1_PIN = 4;
        const char FTM0_CH7_PIN = 5;
        const char FTM0_CH4_PIN = 6;

        switch(PWMOut[OUT])
        {
          case FTM0_CH4_PIN: // PTD4, FTM0_CH4
            FTM0_C4V = ((uint32_t)value * (uint32_t)(FTM0_MOD + 1)) >> (uint32_t)BITS;
            // FTM_PINCFG(FTM0_CH4_PIN) = CORE_PIN6_CONFIG
            CORE_PIN6_CONFIG = PORT_PCR_MUX(4) | PORT_PCR_DSE | PORT_PCR_SRE;
            break;
          case FTM0_CH7_PIN: // PTD7, FTM0_CH7
            FTM0_C7V = ((uint32_t)value * (uint32_t)(FTM0_MOD + 1)) >> (uint32_t)BITS;
            // FTM_PINCFG(FTM0_CH7_PIN) =
            CORE_PIN5_CONFIG = PORT_PCR_MUX(4) | PORT_PCR_DSE | PORT_PCR_SRE;
            break;
          case FTM1_CH0_PIN: // PTA12, FTM1_CH0
            FTM1_C0V = ((uint32_t)value * (uint32_t)(FTM1_MOD + 1)) >> (uint32_t)BITS;
            // FTM_PINCFG(FTM1_CH0_PIN)
            CORE_PIN3_CONFIG = PORT_PCR_MUX(3) | PORT_PCR_DSE | PORT_PCR_SRE;
            break;
          case FTM1_CH1_PIN: // PTA13, FTM1_CH1
            FTM1_C1V = ((uint32_t)value * (uint32_t)(FTM1_MOD + 1)) >> (uint32_t)BITS;
            // FTM_PINCFG(FTM1_CH1_PIN)
            CORE_PIN4_CONFIG = PORT_PCR_MUX(3) | PORT_PCR_DSE | PORT_PCR_SRE;
            break;
        }
    }

    void processCommand(unsigned short command, unsigned int payload);
    void processRequest();

    void resetVoltages();

    void thermalWarning();
    void timeout();
    void configError();
    void setLED(char ledMask = 0b0000);

private:
    const char PWMOut[4] = { 3,  4,  5,  6};
    //const char LEDOut[4] = {9, 21, 22, 23}; // Wiring for ASB
    const char LEDOut[4] = {23, 22, 21, 9}; // Wiring for HVA

    const char AnalogIn[4] = {14, 15, 16, 17};
    const char HeatIn = 20;


    wlTactileUtil() { targetDMA = nullptr; }
    wlTactileUtil(const wlTactileUtil&);
    wlTactileUtil& operator = (const wlTactileUtil&);

    void mainLoop();


    char status;
    char raisePanic;

    IntervalTimer samplingTimer;
    void(*innerLoop)();
    unsigned long samplingSleep;

    bool spiInitialized = false;
    DMAChannel dma1;

    volatile unsigned char currentRunlevel;

    float idle_t;
    unsigned int dacBits;
    unsigned int dacFreq;

    unsigned short lastCommand;

    volatile unsigned char * volatile targetDMA; unsigned int targetDMASize;
    volatile unsigned char * volatile crcDMA; unsigned int crcDMASize;


    void setLED(SystemLED led, bool value);
};

#endif // WLTACTILEUTIL_H
