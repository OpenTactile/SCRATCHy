#include "wltactileutil.h"
#include <FastCRC.h>

#define HWSERIAL         Serial
#define BAUDRATE         9600

//#define DEBUG

// Restart Teensy: https://forum.pjrc.com/threads/24304-_reboot_Teensyduino()-vs-_restart_Teensyduino()?p=35985&viewfull=1#post35985
#define RESTART_ADDR       0xE000ED0C
#define READ_RESTART()     (*(volatile uint32_t *)RESTART_ADDR)
#define WRITE_RESTART(val) ((*(volatile uint32_t *)RESTART_ADDR) = (val))

void restart()
{
    WRITE_RESTART(0x5FA0004);
}

const unsigned long dacFreqTable[17] = {
         0, //  N/A
         0, //  N/A
  12000000, //  2 Bit PWM
   6000000, //  3 Bit PWM
   3000000, //  4 Bit PWM
   1500000, //  5 Bit PWM
    750000, //  6 Bit PWM
    375000, //  7 Bit PWM
    187500, //  8 Bit PWM
     93750, //  9 Bit PWM
     46875, // 10 Bit PWM
     23437, // 11 Bit PWM
     11718, // 12 Bit PWM
      5859, // 13 Bit PWM
      2929, // 14 Bit PWM
      1464, // 15 Bit PWM
       732, // 16 Bit PWM
};


volatile bool barrier = false;

void syncStep()
{
  noInterrupts();
  // Notify user if last loop took too long
  if(!barrier)
    wlTactileUtil::instance().timeout();

  // Release Lock
  barrier = false;
  interrupts();
}

void raiseThermal()
{
    noInterrupts();
    wlTactileUtil::instance().thermalWarning();
    interrupts();
}

void wlTactileUtil::init(void(*loop)())
{
    status = 0b0000;
    raisePanic = 0b1111;    

    //delay(2000);

    innerLoop = loop;

    // Configure outputs

    // PWM
    for(int n = 0; n < 4; n++)
        pinMode(PWMOut[n], OUTPUT);

    // System LEDs
    for(int n = 0; n < 4; n++)
        pinMode(LEDOut[n], OUTPUT);

    // Analog inputs
    for(int n = 0; n < 4; n++)
        pinMode(AnalogIn[n], INPUT);


    // Setup Heatwarning interrupt
    pinMode(HeatIn, INPUT);
    //attachInterrupt(HeatIn, raiseThermal, RISING);

    // Set Voltage to 0V
    R2_SetDACResolution(9);
    resetVoltages();

    // Short animation
    {
        setLED(0);
        digitalWrite(LEDOut[3],HIGH);
        delay(100);
        digitalWrite(LEDOut[2],HIGH);
        delay(100);
        digitalWrite(LEDOut[1],HIGH);
        delay(100);
        digitalWrite(LEDOut[0],HIGH);
        delay(100);
        setLED(0);
        delay(100);
        setLED(0b1111);
        delay(100);
        setLED(0);
        delay(100);
        setLED(0b1111);
        delay(100);
        setLED(0);
        delay(200);
    }

    currentRunlevel = 255;

#ifdef DEBUG
    HWSERIAL.print("dmaAddress#2: ");
    HWSERIAL.println((unsigned int)(&targetDMA[0]), HEX);
    HWSERIAL.print("dmaLength#2: ");
    HWSERIAL.println(targetDMASize, DEC);
#endif

    mainLoop();
}

void wlTactileUtil::initSPI2DMA(volatile unsigned char * volatile dmaMemory, unsigned int dmaMemoryLength)
{
    dma1.disable();

    delay(1000);
    if(!spiInitialized)
    {
        SIM_SCGC6 |= SIM_SCGC6_SPI0;
        delay(1000);
        SPI0_MCR = SPI_MCR_HALT;// | SPI_MCR_MDIS;
        SPI0_MCR |= SPI_MCR_DIS_RXF | SPI_MCR_DIS_TXF;  // Disable FIFOs
        SPI0_CTAR0_SLAVE = 0;
        SPI0_CTAR0_SLAVE |= SPI_CTAR_FMSZ(8 - 1);       // 8bit Mode
        SPI0_CTAR0_SLAVE = (SPI0_CTAR0_SLAVE & ~(SPI_CTAR_CPOL)); // SPI Mode 1
        SPI0_RSER = SPI_RSER_TFFF_DIRS | SPI_RSER_TFFF_RE |
                        SPI_RSER_RFDF_DIRS | SPI_RSER_RFDF_RE;
        // Configure muxes to use the right pins for SPI
        CORE_PIN13_CONFIG = PORT_PCR_MUX(2);
        CORE_PIN11_CONFIG = PORT_PCR_DSE | PORT_PCR_MUX(2);
        CORE_PIN12_CONFIG = PORT_PCR_MUX(2);
        CORE_PIN10_CONFIG = PORT_PCR_MUX(2);
        SPI0_MCR &= ~SPI_MCR_HALT; // Start

        dma1.source(SPI0_POPR);
    }
    dma1.destinationBuffer(dmaMemory, dmaMemoryLength);

#ifdef DEBUG
    HWSERIAL.print("dmaAddress: ");
    HWSERIAL.println((unsigned int)(&targetDMA[0]), HEX);

    HWSERIAL.println((unsigned int)(dmaMemory), HEX);
    HWSERIAL.print("dmaLength: ");
    HWSERIAL.println(dmaMemoryLength, DEC);
#endif

    if(!spiInitialized)
    {
        dma1.transferSize(1); // 8bit
        dma1.triggerAtHardwareEvent(DMAMUX_SOURCE_SPI0_RX);
    }
    dma1.enable();

    spiInitialized = true;
}


void wlTactileUtil::Init_Idle()
{
    idle_t += 0.05;
    if(idle_t > 2.0 * 3.1415)
        idle_t = 0.0;
    for(int n = 0; n < 4; n++)
    {
        float p = 255 * sin( 0.25* 3.1415 * n + 5*3.1415*sin(idle_t) + 3.1415) + 255;

        analogWrite(LEDOut[n], p);
    }

    delay(30);
}

void wlTactileUtil::Rp_Idle()
{
    setLED(0b0000);
    delay(50);
    setLED(status);
    delay(100);
    setLED(0b0000);
    delay(50);
    setLED(status);
    delay(800);
}

void wlTactileUtil::R4_Idle()
{
    idle_t += 0.025;
    for(int n = 0; n < 4; n++)
        analogWrite(LEDOut[n], 255 * sin(idle_t + 3.1415*0.25*n) + 255);
    delay(30);
}

void receiveI2C(size_t len)
{    
    noInterrupts();

    //Prevent timeout if interrupted
    wlTactileUtil::instance().R3_StopMainLoop();

#ifdef DEBUG
    HWSERIAL.print("\n\rI2C Received, length: ");
    HWSERIAL.println(len);
#endif

    unsigned short command = -1;
    unsigned int payload = 0;

    if(Wire.available() >= 1)
    {
        command = Wire.readByte();

#ifdef DEBUG
    HWSERIAL.print("Command: ");
    HWSERIAL.print(command, BIN);
    HWSERIAL.print(" (");
    HWSERIAL.print(command, HEX);
    HWSERIAL.println(")");
#endif
    }

    if(Wire.available() >= 4)
    {

        unsigned char pld[4] = {Wire.readByte(), Wire.readByte(),
                                Wire.readByte(), Wire.readByte()};

        payload = (((unsigned int)pld[0]) << (8*3)) +
                  (((unsigned int)pld[1]) << (8*2)) +
                  (((unsigned int)pld[2]) << (8*1)) +
                  (((unsigned int)pld[3]) << (8*0));


#ifdef DEBUG
    HWSERIAL.print("Payload: ");
    HWSERIAL.print(payload, DEC);
    HWSERIAL.print(" (");
    HWSERIAL.print(payload, HEX);
    HWSERIAL.println(")");
#endif
    }


    while(Wire.available())
        Wire.readByte(); // Drop remaining data


    if(command != -1)
        wlTactileUtil::instance().processCommand(command, payload);

    barrier = true;
    interrupts();
}

void requestI2C(void)
{
    noInterrupts();
    wlTactileUtil::instance().processRequest();
    barrier = true;
    interrupts();
}

void wlTactileUtil::initI2C(unsigned int address)
{
    Wire.begin(I2C_SLAVE, address, I2C_PINS_18_19, I2C_PULLUP_EXT, I2C_RATE_100);
    Wire.onReceive(receiveI2C);
    Wire.onRequest(requestI2C);

#ifdef DEBUG
    HWSERIAL.print("I2C initialized, address: ");
    HWSERIAL.println(address);
#endif
}

void wlTactileUtil::processCommand(unsigned short command, unsigned int payload)
{
    lastCommand = command;

    // Global commands:

    if(command == SystemRequest::Reset)
    {
        restart();
        return;
    }

    if(command == SystemRequest::Alive)
    {
#ifdef DEBUG
    HWSERIAL.println("Alive requested");
#endif
        return;
    }
    if(command == SystemRequest::Status)
    {
#ifdef DEBUG
    HWSERIAL.println("Status requested");
#endif
        return;
    }
    if(command == SystemRequest::StatusReset)
    {
        status = 0;
        return;
    }

    switch(currentRunlevel)
    {
    case 255:
        if(command == SystemRequest::InitSystem)
            currentRunlevel = 0;
        break;

    case 0: // Address initialization
        if(command == SystemRequest::FinishR0)
            currentRunlevel = 1;

        if(command == SystemRequest::SetI2CAddress)
        {
            // Set new Adress
            bool rightOne = false;

#ifdef DEBUG
    HWSERIAL.print("SCL is: ");
    HWSERIAL.println(digitalRead(10));
#endif
            rightOne = (digitalRead(10) == LOW);
            if(rightOne)
                initI2C(payload);
        }

        break;

    case 1: // SPI test
        if(command == SystemRequest::FinishR1)
            currentRunlevel = 2;
        if(command == SystemRequest::SPIPrepare)
        {
            asm volatile ("dsb");
            dma1.destinationBuffer(crcDMA, crcDMASize);
        }

        break;

    case 2: // Configuration
        if(command == SystemRequest::FinishR2)
            currentRunlevel = 3;

        if(command == SystemRequest::SetDACResolution)
            R2_SetDACResolution(payload);

        if(command == SystemRequest::SetPanicMask)
            R2_SetPanicMask(payload);

        if(command == SystemRequest::SetSamplingRate)
            R2_SetSamplingRate(payload);

        break;

    case 3: // Signalloop
        if(command == SystemRequest::FinishR3)
            currentRunlevel = 4;

        break;

    case 5: // Recovery
        if(command == SystemRequest::FinishRecovery)
            currentRunlevel = 3;

        break;
    }
}

void wlTactileUtil::processRequest()
{
    // The current status can be acquired at any time
    if(lastCommand == SystemRequest::Status)
    {        
        Wire.write(status);
        return;
    }

    switch(currentRunlevel)
    {
    case 1: // SPI test
        if(lastCommand == SystemRequest::SPIChecksum)
        {
            unsigned long crc = checkCRC32();
            Wire.write(&((byte*)(&crc))[0], 4);
            return;
        }
        break;
    }
}

unsigned long wlTactileUtil::checkCRC32()
{
    FastCRC32 crc32;
    #ifdef DEBUG
    HWSERIAL.println("\nCRC32 Data:");
    for(int n = 0; n < crcDMASize; n++)
    {
        HWSERIAL.print((unsigned int)(crcDMA[n]), DEC);
        HWSERIAL.print(" ");
    }
    #endif

    return crc32.crc32(const_cast<const unsigned char*>(crcDMA), crcDMASize);
}

volatile unsigned char * volatile wlTactileUtil::requestDMABuffer(unsigned int bufferSize)
{
#ifdef DEBUG
    HWSERIAL.begin(BAUDRATE);
    HWSERIAL.println("DMA request");
#endif

    if(targetDMA)
        delete[] targetDMA;

    targetDMA = new volatile unsigned char[bufferSize];
    targetDMASize = bufferSize;

#ifdef DEBUG
    HWSERIAL.println("Initialized DMA buffer:");
    HWSERIAL.print("dmaAddress: ");
    HWSERIAL.println((unsigned int)(&targetDMA[0]), HEX);
    HWSERIAL.print("dmaLength: ");
    HWSERIAL.println(targetDMASize, DEC);
#endif

    return &targetDMA[0];
}

void wlTactileUtil::mainLoop()
{
    while(true)
    {

#ifdef DEBUG
    HWSERIAL.print("=== Runlevel is now: ");
    HWSERIAL.print(currentRunlevel, DEC);
    HWSERIAL.println(" ===");
#endif
        switch(currentRunlevel)
        {
        case 255:
            idle_t = 0.0;
            analogWriteResolution(10);
            for(int n = 0; n < 4; n++)
                analogWriteFrequency(LEDOut[n], dacFreqTable[10]);

            initI2C(0);
            resetVoltages();

            while(currentRunlevel == 255)
                Init_Idle();
            break;

        case 0: // Address initialization
            for(int n = 0; n < 4; n++)
                pinMode(LEDOut[n], OUTPUT);

            // Set SCL to INPUT
            pinMode(10, INPUT);
            initI2C(0);

            setLED(0b1000);
            while(currentRunlevel == 0) delay(50);
            break;

        case 1: // SPI test
            setLED(0b1100);
            //R2_SetDefaults(); // Without this, DMA fails? WHY???
            //R1_PrepareDMATest();
            while(currentRunlevel == 1) delay(50);
            break;

        case 2: // Configuration
            setLED(0b1110);            
            R2_SetDefaults();            
            while(currentRunlevel == 2) delay(50);
            break;

        case 3: // Signalloop
            setLED(0b1111);            
            delay(250);
            setLED(0b0000);
            delay(250);
            setLED(0b1111);
            delay(250);
            setLED(0b0000);
            R3_StartMainLoop();
            while(currentRunlevel == 3)
            {
                while(barrier);
                innerLoop();

                // SPI Safety Reset
                if(digitalReadFast(10) == HIGH)
                {
                    dma1.destinationBuffer(targetDMA, targetDMASize);                                        
                }
#ifdef DEBUG
    HWSERIAL.print(SPI0_POPR, DEC);
    HWSERIAL.print(" -> ");
    //asm volatile ("dsb");
    HWSERIAL.print(*((unsigned short*)&targetDMA[0]), DEC);
    HWSERIAL.print(" - ");
    HWSERIAL.println(*((unsigned short*)&targetDMA[2]), DEC);
#endif
                barrier = true;
            }
            R3_StopMainLoop();
            break;

        case 4: // Shutdown
            for(int n = 0; n < 4; n++)
                analogOut(n, 0);

            idle_t = 0.0;
            analogWriteResolution(10);
            for(int n = 0; n < 4; n++)
                analogWriteFrequency(LEDOut[n], dacFreqTable[10]);
            resetVoltages();
            while(currentRunlevel == 4)
                R4_Idle();
            break;

        case 5: // Reserved
            break;

        case 6: // Emergency shutdown
            samplingTimer.end();

            for(int n = 0; n < 4; n++)
                analogOut(n, 0);

            resetVoltages();

            while(true)
                Rp_Idle();
            break;
        }
    }
}

void wlTactileUtil::R2_SetDefaults()
{
    R2_SetPanicMask(0b1100);
    R2_SetDACResolution(9);
    R2_SetSamplingRate(40); // 25kHz default
    //R2_SetSamplingRate(62); // 16kHz default
}

void wlTactileUtil::R2_SetSamplingRate(unsigned int samplingRate)
{    
    samplingSleep = samplingRate;
}

void wlTactileUtil::R2_SetDACResolution(unsigned char dacBits)
{
    const unsigned int dacFrequency = dacFreqTable[dacBits];
    this->dacBits = dacBits;
    this->dacFreq = dacFrequency;

    analogWriteResolution(dacBits);
    for(int n = 0; n < 4; n++)
        analogWriteFrequency(PWMOut[n], dacFrequency);
}

void wlTactileUtil::R2_SetPanicMask(unsigned char mask)
{
    raisePanic = mask;
}

void wlTactileUtil::R1_PrepareDMATest()
{
    // Prepare dataBuffer for DMA testing
    crcDMASize = 128;
    crcDMA = new volatile unsigned char[crcDMASize];

    for(unsigned int n = 0; n < crcDMASize; n++)
        crcDMA[n] = 0;

    initSPI2DMA(crcDMA, crcDMASize);
}

void wlTactileUtil::R3_StartMainLoop()
{
    initSPI2DMA(targetDMA, targetDMASize);
    delay(100);
    delete[] crcDMA;
    barrier = true;
    if(innerLoop)
        samplingTimer.begin(syncStep, samplingSleep);
}


void wlTactileUtil::R3_StopMainLoop()
{
    barrier = true;
    samplingTimer.end();
    R2_SetPanicMask(0b0000);
    resetVoltages();
}

void wlTactileUtil::setLED(char ledMask)
{
    for(int n = 0; n < 4; n++)
    {
        if(ledMask & (1 << n))
            digitalWrite(LEDOut[n], HIGH);
        else
            digitalWrite(LEDOut[n], LOW);
    }
}

void wlTactileUtil::setLED(SystemLED led, bool value)
{
    digitalWrite(LEDOut[static_cast<int>(led)], value? HIGH : LOW);
}

void wlTactileUtil::thermalWarning()
{
    char newStatus = static_cast<char>(SystemStatus::ThermalWarning);
    status |= newStatus;
    if((raisePanic & newStatus) > 0)
    {
        currentRunlevel = 6;
        R3_StopMainLoop();
        interrupts();
    }
    else
        setLED(status);
}

void wlTactileUtil::timeout()
{
    char newStatus = static_cast<char>(SystemStatus::NotInSync);

    status |= newStatus;
    if((raisePanic & newStatus) > 0)
    {
        currentRunlevel = 6;
        R3_StopMainLoop();
        interrupts();
    }
    else
        setLED(status);
}

void wlTactileUtil::configError()
{
    char newStatus = static_cast<char>(SystemStatus::ConfigInvalid);
    status |= newStatus;
    if((raisePanic & newStatus) > 0)
        currentRunlevel = 6;
    else
        setLED(status);
}

void wlTactileUtil::resetVoltages()
{
    analogOutQ15<0>(0);
    analogOutQ15<1>(0);
    analogOutQ15<2>(0);
    analogOutQ15<3>(0);
}

