#ifndef IMAGEHANDLINGOLED_H
#define IMAGEHANDLINGOLED_H

#include <QImage>

namespace ImageHandlingOLED
{
    extern bool initDone;
    extern QImage actlImage;
    void init(int resetGPIOpin = 7);
    void clear(int color = 0);
    void drawText(int x, int y, const QString& str, QFont font);
    void drawImage(int x, int y, bool center, const QString& fn);
    void display();
    void free();
}

#endif // IMAGEHANDLINGOLED_H
