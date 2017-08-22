#include "iowrap.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>


#include <iostream>
#include <unistd.h>
#include <sys/ioctl.h>
#include <fcntl.h>

namespace IO
{
    void GPIOSetDirection(unsigned int pin, GPIODirection direction)
    {

    }

    void GPIOHigh(unsigned int pin)
    {

    }

    void GPIOLow(unsigned int pin)
    {

    }

    bool GPIOIsLow(unsigned int pin)
    {
        return false;
    }

    bool GPIOIsHigh(unsigned int pin)
    {
        return false;
    }

    void GPIOInit()
    {

    }

    void GPIOFree()
    {

    }

    void GPIOSetAddress(unsigned int address)
    {

    }

    void GPIOSetBroadCast(bool value)
    {

    }

    void SPIInit()
    {

    }

    void SPIFree()
    {

    }

    void SPISetDivider(unsigned int divider)
    {

    }

    unsigned int SPIGetCurrentSpeed()
    {
        return 0;
    }

    void SPISend(const uint16_t *data, int size)
    {

    }

    void I2CInit()
    {
    }

    void I2CFree()
    {

    }

    bool I2CSetAddress(int address)
    {
        return false;
    }

    char I2CReadByte(int cmd)
    {
        return 0;
    }

    bool I2CWriteByte(int cmd, unsigned char buffer)
    {
        return false;
    }

    bool I2CReadBlock(int cmd, uint16_t length, unsigned char *buffer)
    {
        return false;
    }

    bool I2CWriteBlock(int cmd, uint16_t length, const unsigned char* buffer)
    {
        return false;
    }

}

