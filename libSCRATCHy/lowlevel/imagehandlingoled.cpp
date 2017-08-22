#include "imagehandlingoled.h"

#include <QPainter>
#include <QRgb>

#include <assert.h>

#include "oleddisplay.h"

namespace ImageHandlingOLED
{
    bool initDone;
    QImage actlImage;

    void init(int resetGPIOpin, const QString& i2cdevice)
    {
        assert(!initDone);
        OLEDDisplay::init(resetGPIOpin, i2cdevice);
        actlImage = QImage(OLEDDisplay::width,OLEDDisplay::height,QImage::Format_Mono);
        //actlImage = QImage(OLEDDisplay::width,OLEDDisplay::height,QImage::Format_RGB888);
        initDone = true;
        clear();
    }

    void clear(int color)
    {
        assert(initDone);
        actlImage.fill(color);
    }

    void drawText(int x, int y, const QString& str,QFont font)
    {
        assert(initDone);
        QPoint point(x,y);
        QPainter painter(&actlImage);
        painter.setFont(font);
        painter.drawText(point,str);
        painter.end();
    }

    void drawImage(int x, int y, bool center, const QString& fn)
    {
        assert(initDone);
        QImage newImage(fn);
        if (center)
        {
            x = x + (OLEDDisplay::width - newImage.width()) / 2;
            y = y + (OLEDDisplay::height - newImage.height()) / 2;
        }
        QPainter painter(&actlImage);
        painter.drawImage(x,y,newImage);
        painter.end();
    }

    void display()
    {
        assert(initDone);
        int buffersize = OLEDDisplay::width * OLEDDisplay::pages;
        uchar* buffer = new uchar[buffersize];
        int index = 0;
        for (int p = 0; p < OLEDDisplay::pages; p++) for (int x = 0; x < OLEDDisplay::width; x++)
        {
            uchar bits = 0;
            for (uchar bit = 0; bit < 8; bit++)
            {
                bits = bits << 1;
                uchar b = 1;
                QRgb pix = actlImage.pixel(x, p*8+7-bit);
                //bool black = (pix.red() + pix.green() + pix.blue()) < 384;
                bool black = (pix == 0xff000000); //AARRGGBB
                //if (p == 2) black = false;
                //black = true;
                if (black) b = 0;
                bits |= b;
            }
            buffer[index] = bits;
            assert((p * OLEDDisplay::width + x) == index);
            index++;
        }
        OLEDDisplay::drawPic(buffer);
        delete[] buffer;
    }

    void free()
    {
        assert(initDone);
        OLEDDisplay::free();
        initDone = false;
    }
}

