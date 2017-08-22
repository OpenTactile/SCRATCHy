#ifndef OLEDDISPLAY_H
#define OLEDDISPLAY_H

#include <QString>

#define SSD1306_EXTERNALVCC 0x1
#define SSD1306_SWITCHCAPVCC 0x2

namespace OLEDDisplay
{
    extern int width;
    extern int height;
    extern bool initDone;
    extern int pages;
    void init(int resetGPIOpin = 7, const QString &i2cdevice = "/dev/i2c-1", int vccstate = SSD1306_SWITCHCAPVCC);
    void drawPic(const unsigned char *data);
    void set_contrast(unsigned char contrast);
    void dim(bool dim);
    void free();
}

#endif // OLEDDISPLAY_H
