#include "iowrap.h"
#include <wiringPi.h>
//#include <wiringPiSPI.h>
#include <bcm2835.h>

#include <assert.h>
#include <stdlib.h>
#include <string.h>


#include <iostream>
extern "C" {
    #include <linux/i2c-dev.h>
}
#include <unistd.h>
#include <sys/ioctl.h>
#include <fcntl.h>

namespace IO
{
    int gpioCnt = 0;
    int gpioPins[7] = {21,22,23,24,25,29,28};
    int gpioBroadcastPin = 27;

    void GPIOSetDirection(unsigned int pin, GPIODirection direction)
    {
        pullUpDnControl(pin, PUD_OFF);
        if(direction == GPIODirection::input)
            pinMode(pin, INPUT);
        else if(direction == GPIODirection::output)
            pinMode(pin, OUTPUT);
        else if(direction == GPIODirection::input_pulldown)
        {
            pinMode(pin, INPUT);
            pullUpDnControl(pin, PUD_DOWN);
        }
        else if(direction == GPIODirection::input_pullup)
        {
            pinMode(pin, INPUT);
            pullUpDnControl(pin, PUD_UP);
        }
    }

    void GPIOHigh(unsigned int pin)
    {
        digitalWrite(pin, HIGH);
    }

    void GPIOLow(unsigned int pin)
    {
        digitalWrite(pin, LOW);
    }

    bool GPIOIsLow(unsigned int pin)
    {
        return (digitalRead(pin) == LOW);
    }

    bool GPIOIsHigh(unsigned int pin)
    {
        return (digitalRead(pin) == HIGH);
    }

    void GPIOInit()
    {
        if (gpioCnt == 0)
        {
            wiringPiSetup();
            for (int i = 0; i  < 7; i++)
                GPIOSetDirection(gpioPins[i], IO::output);
            GPIOSetDirection(gpioBroadcastPin, IO::output);
        }
        gpioCnt++;
    }

    void GPIOFree()
    {
        assert(gpioCnt > 0);
        gpioCnt--;
    }

    void GPIOSetAddress(unsigned int address)
    {
        for (int i = 0; i < 7; i++)
        {
            bool high = (bool)(((1 << i) & address) >> i);
            if (high)
                GPIOHigh(gpioPins[i]);
            else
                GPIOLow(gpioPins[i]);
        }
    }

    void GPIOSetBroadCast(bool value)
    {
        if(value)
            GPIOLow(gpioBroadcastPin);
        else
            GPIOHigh(gpioBroadcastPin);
    }

    int spiCnt = 0;
    unsigned int spiDiv = 0;

    void SPIInit()
    {
        if(spiCnt == 0)
        {
            bool initialized = bcm2835_init() && bcm2835_spi_begin();
            if(!initialized)
                return;

            bcm2835_spi_setBitOrder(BCM2835_SPI_BIT_ORDER_MSBFIRST);      // The default
            bcm2835_spi_setDataMode(BCM2835_SPI_MODE1);                   // The default

            SPISetDivider(128);

            bcm2835_spi_chipSelect(BCM2835_SPI_CS1); // Hacky - but we do not want to use the rpi's CS
            bcm2835_spi_setChipSelectPolarity(BCM2835_SPI_CS0, LOW);
            bcm2835_spi_setChipSelectPolarity(BCM2835_SPI_CS1, LOW);
        }
        spiCnt++;
    }

    void SPIFree()
    {
        if(spiCnt == 0)
        {
            bcm2835_spi_end();
            bcm2835_close();
        }
        spiCnt--;
    }

    void SPISetDivider(unsigned int divider)
    {
        if(divider < BCM2835_SPI_CLOCK_DIVIDER_16) // Prevent damage to Teensy
            divider = BCM2835_SPI_CLOCK_DIVIDER_16;
        bcm2835_spi_setClockDivider(divider);
        spiDiv = divider;
    }

    unsigned int SPIGetCurrentSpeed()
    {
        const int baseFrequency = 400000; // 400 MHz for RPi3
        return baseFrequency / spiDiv;
    }

    void SPISend(const uint16_t *data, int size)
    {
        usleep(20);
        const char* dataChar = (const char*)data;
        bcm2835_spi_writenb(const_cast<char*>(dataChar), size);
        usleep(20);
    }

    int i2cCnt = 0;
    int i2cFd;

    void I2CInit() //
    {
        if (i2cCnt == 0)
        {
            i2cFd = open("/dev/i2c-1",O_RDWR);
            assert(i2cFd >= 0);
        }
        i2cCnt++;
    }

    void I2CFree()
    {
        assert(i2cCnt > 0);
        if (i2cCnt == 1)
        {
            int res = close(i2cFd);
            assert(res >= 0);
        }
        i2cCnt--;
    }

    bool I2CSetAddress(int address)
    {
        assert(i2cCnt > 0);
        int res = ioctl(i2cFd, I2C_SLAVE, address);
        return res >= 0;
    }

    char I2CReadByte(int cmd)
    {
        assert(i2cCnt > 0);
        int res = i2c_smbus_read_byte_data(i2cFd, cmd);
        return static_cast<char>(res);
    }

    bool I2CWriteByte(int cmd, unsigned char buffer)
    {
        assert(i2cCnt > 0);
        int res = i2c_smbus_write_byte_data(i2cFd, cmd, buffer);
        return res >= 0;
    }

    bool I2CReadBlock(int cmd, uint16_t length, unsigned char *buffer)
    {
        assert(i2cCnt > 0);
        int res = i2c_smbus_read_i2c_block_data(i2cFd, cmd, length, buffer);
        return res >= 0;
    }

    bool I2CWriteBlock(int cmd, uint16_t length, const unsigned char* buffer)
    {
        assert(i2cCnt > 0);
        int res = i2c_smbus_write_i2c_block_data(i2cFd, cmd, length, buffer);
        return res >= 0;
    }

}

