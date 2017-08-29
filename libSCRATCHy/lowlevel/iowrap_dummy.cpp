#include "scratchy/iowrap.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include <iostream>
#include <unistd.h>
#include <sys/ioctl.h>
#include <fcntl.h>

#include <QtGlobal>

namespace IO
{
    void GPIOSetDirection(unsigned int pin, GPIODirection direction)
    {
        Q_UNUSED(pin); Q_UNUSED(direction);
    }

    void GPIOHigh(unsigned int pin)
    {
        Q_UNUSED(pin);
    }

    void GPIOLow(unsigned int pin)
    {
        Q_UNUSED(pin);
    }

    bool GPIOIsLow(unsigned int pin)
    {
        Q_UNUSED(pin);
        return false;
    }

    bool GPIOIsHigh(unsigned int pin)
    {
        Q_UNUSED(pin);
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
        Q_UNUSED(address);
    }

    void GPIOSetBroadCast(bool value)
    {
        Q_UNUSED(value);
    }

    void SPIInit()
    {

    }

    void SPIFree()
    {

    }

    void SPISetDivider(unsigned int divider)
    {
        Q_UNUSED(divider);
    }

    unsigned int SPIGetCurrentSpeed()
    {
        return 0;
    }

    void SPISend(const uint16_t *data, int size)
    {
        Q_UNUSED(data); Q_UNUSED(size);
    }

    void I2CInit()
    {
    }

    void I2CFree()
    {

    }

    bool I2CSetAddress(int address)
    {
        Q_UNUSED(address);
        return false;
    }

    char I2CReadByte(int cmd)
    {
        Q_UNUSED(cmd);
        return 0;
    }

    bool I2CWriteByte(int cmd, unsigned char buffer)
    {
        Q_UNUSED(cmd); Q_UNUSED(buffer);
        return false;
    }

    bool I2CReadBlock(int cmd, uint16_t length, unsigned char* buffer)
    {
        Q_UNUSED(cmd); Q_UNUSED(length); Q_UNUSED(buffer);
        return false;
    }

    bool I2CWriteBlock(int cmd, uint16_t length, const unsigned char* buffer)
    {
        Q_UNUSED(cmd); Q_UNUSED(length); Q_UNUSED(buffer);
        return false;
    }

}

