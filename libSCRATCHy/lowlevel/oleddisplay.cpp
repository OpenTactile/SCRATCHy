#include "oleddisplay.h"

#include <unistd.h>
#include <assert.h>

#include "scratchy/iowrap.h"

//Constants
#define SSD1306_I2C_ADDRESS 0x3C    //011110+SA0+RW - 0x3C or 0x3D
#define SSD1306_SETCONTRAST 0x81
#define SSD1306_DISPLAYALLON_RESUME 0xA4
#define SSD1306_DISPLAYALLON 0xA5
#define SSD1306_NORMALDISPLAY 0xA6
#define SSD1306_INVERTDISPLAY 0xA7
#define SSD1306_DISPLAYOFF 0xAE
#define SSD1306_DISPLAYON 0xAF
#define SSD1306_SETDISPLAYOFFSET 0xD3
#define SSD1306_SETCOMPINS 0xDA
#define SSD1306_SETVCOMDETECT 0xDB
#define SSD1306_SETDISPLAYCLOCKDIV 0xD5
#define SSD1306_SETPRECHARGE 0xD9
#define SSD1306_SETMULTIPLEX 0xA8
#define SSD1306_SETLOWCOLUMN 0x00
#define SSD1306_SETHIGHCOLUMN 0x10
#define SSD1306_SETSTARTLINE 0x40
#define SSD1306_MEMORYMODE 0x20
#define SSD1306_COLUMNADDR 0x21
#define SSD1306_PAGEADDR 0x22
#define SSD1306_COMSCANINC 0xC0
#define SSD1306_COMSCANDEC 0xC8
#define SSD1306_SEGREMAP 0xA0
#define SSD1306_CHARGEPUMP 0x8D

//Scrolling constants
#define SSD1306_ACTIVATE_SCROLL 0x2F
#define SSD1306_DEACTIVATE_SCROLL 0x2E
#define SSD1306_SET_VERTICAL_SCROLL_AREA 0xA3
#define SSD1306_RIGHT_HORIZONTAL_SCROLL 0x26
#define SSD1306_LEFT_HORIZONTAL_SCROLL 0x27
#define SSD1306_VERTICAL_AND_RIGHT_HORIZONTAL_SCROLL 0x29
#define SSD1306_VERTICAL_AND_LEFT_HORIZONTAL_SCROLL 0x2A

namespace OLEDDisplay
{
    int width;
    int height;
    bool initDone = false;
    int pages;
    int _resetGPIOpin;
    unsigned char _contrast;
    int _vccstate;

    void command(int cmd);
    void reset();

    void init(int resetGPIOpin, int vccstate)
    {
        assert(!initDone);
        IO::GPIOInit();
        IO::I2CInit();
        width = 128;
        height = 32;
        pages = height / 8;
        _resetGPIOpin = resetGPIOpin;

        IO::GPIOSetDirection(_resetGPIOpin, IO::output);
        _vccstate = vccstate;
        reset();
        IO::I2CSetAddress(SSD1306_I2C_ADDRESS);

        command(SSD1306_DISPLAYOFF);         // 0xAE
        command(SSD1306_SETDISPLAYCLOCKDIV); // 0xD5
        command(0x80);                       // the suggested ratio 0x80
        command(SSD1306_SETMULTIPLEX);       // 0xA8
        command(0x1F);
        command(SSD1306_SETDISPLAYOFFSET);   // 0xD3
        command(0x0);                        // no offset
        command(SSD1306_SETSTARTLINE | 0x0); // line #0
        command(SSD1306_CHARGEPUMP);         // 0x8D
        if (_vccstate == SSD1306_EXTERNALVCC) command(0x10);
        else command(0x14);
        command(SSD1306_MEMORYMODE);         // 0x20
        command(0x00);                       // 0x0 act like ks0108
        command(SSD1306_SEGREMAP | 0x1);
        command(SSD1306_COMSCANDEC);
        command(SSD1306_SETCOMPINS);         // 0xDA
        command(0x02);
        command(SSD1306_SETCONTRAST);        // 0x81
        command(0x8F);
        command(SSD1306_SETPRECHARGE);       // 0xd9
        if (_vccstate == SSD1306_EXTERNALVCC) command(0x22);
        else command(0xF1);
        command(SSD1306_SETVCOMDETECT);      // 0xDB
        command(0x40);
        command(SSD1306_DISPLAYALLON_RESUME);// 0xA4
        command(SSD1306_NORMALDISPLAY);      // 0xA6

        command(SSD1306_DISPLAYON);
        initDone = true;
    }

    void command(int cmd)
    {
        int ctrl = 0x00;
        IO::I2CWriteByte(ctrl,cmd);
    }

    void reset()
    {
        IO::GPIOHigh(_resetGPIOpin);
        usleep(1000);
        IO::GPIOLow(_resetGPIOpin);
        usleep(10000);
        IO::GPIOHigh(_resetGPIOpin);
    }

    void drawPic(const unsigned char* data)
    {
        assert(initDone);
        IO::I2CSetAddress(SSD1306_I2C_ADDRESS);
        command(SSD1306_COLUMNADDR);
        command(0);              // Column start address. (0 = reset)
        command(width - 1);      // Column end address.
        command(SSD1306_PAGEADDR);
        command(0);              // Page start address. (0 = reset)
        command(pages - 1);      // Page end address.
        int datalen = 16;
        for (int i = 0; i < (width * pages); i += datalen)
        {
            int control = 0x40;            
            IO::I2CWriteBlock(control, datalen, &(data[i]));
        }
    }

    void set_contrast(unsigned char contrast)
    {
        assert(initDone);
        _contrast = contrast;
        IO::I2CSetAddress(SSD1306_I2C_ADDRESS);
        command(SSD1306_SETCONTRAST);
        command(contrast);
    }

    void dim(bool dim)
    {
        assert(initDone);
        _contrast = 0;
        if (!dim)
        {
            if (_vccstate == SSD1306_EXTERNALVCC) _contrast = 0x9F;
            else _contrast = 0xCF;
        }
    }

    void free()
    {
        assert(initDone);
        initDone = false;
        IO::I2CSetAddress(SSD1306_I2C_ADDRESS);
        command(SSD1306_DISPLAYOFF);
        IO::I2CFree();
        IO::GPIOFree();
    }
}

