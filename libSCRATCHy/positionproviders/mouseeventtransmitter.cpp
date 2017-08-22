#include "mouseeventtransmitter.h"

#include "lowlevel/mouseevents.h"

#include <QDebug>

MouseEventTransmitter::MouseEventTransmitter(QObject *parent) :
    QObject(parent)
{

}

bool MouseEventTransmitter::initialize()
{
    QStringList devices = MouseEvents::getDevices();
    QStringList devicesFiltered = devices.filter("USB");
    qDebug() << "Found mouse: " << devices;
    if(devicesFiltered.isEmpty()) return false;
    QString device;
    if(devicesFiltered.isEmpty()) {
        device = devices.first();
    } else {
        device = devicesFiltered.first();
    }

    MouseEvents::init(device);
    return true;
}

void MouseEventTransmitter::update() {
    int x=0;
    int y=0;
    unsigned long time=0.0;
    while(true) {
        MouseEvents::getPos(x,y,time,button);
        emit mouseChanged(x,y,time,button);
    }
}
