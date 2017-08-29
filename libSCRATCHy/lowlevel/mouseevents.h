#ifndef MOUSEEVENTS_H
#define MOUSEEVENTS_H

#include <QString>
#include <QStringList>

namespace MouseEvents
{
    extern int fd;
    extern bool initDone;
    void init(const QString &devname);
    QStringList getDevices();
    void getPos(int &dx, int &dy, bool &button);
    void free();
}

#endif // MOUSEEVENTS_H
