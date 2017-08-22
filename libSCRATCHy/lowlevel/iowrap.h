#ifndef IOWRAP_H
#define IOWRAP_H

// Some wrapper functions to interface the systems io functions
// May connect with e.g. wiringPI or BBBIOLib
#include <cstdint>

namespace IO
{
    enum GPIODirection
    {
        output,
        input,
        input_pullup,
        input_pulldown
    };

    extern int gpioCnt;    
    void GPIOSetDirection(unsigned int pin, GPIODirection direction);
    void GPIOHigh(unsigned int pin);
    void GPIOLow(unsigned int pin);
    bool GPIOIsLow(unsigned int pin);
    bool GPIOIsHigh(unsigned int pin);

    void GPIOInit();
    void GPIOFree();
    void GPIOSetAddress(unsigned int address);
    void GPIOSetBroadCast(bool value);

    extern int spiCnt;
    extern unsigned int spiDiv;
    void SPIInit();
    void SPIFree();
    void SPISetDivider(unsigned int divider);
    unsigned int SPIGetCurrentSpeed(); // In kHz
    void SPISend(const uint16_t *data, int size);

    extern int i2cCnt;
    extern int i2cFd;
    void I2CInit();
    void I2CFree();
    bool I2CSetAddress(int address);
    char I2CReadByte(int cmd);
    bool I2CWriteByte(int cmd, unsigned char buffer);
    bool I2CReadBlock(int cmd, uint16_t length, unsigned char *buffer);
    bool I2CWriteBlock(int cmd, uint16_t length, const unsigned char* buffer);
}

#endif // IOWRAP_H
