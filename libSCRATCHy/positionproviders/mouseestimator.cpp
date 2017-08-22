#include "mouseestimator.h"

#include "lowlevel/mouseevents.h"

#include <QDebug>

MouseEstimator::MouseEstimator() :
    mpd(0.1/4480,0.1/4378),
    massInverse(100),
    stiffness(500),
    damping(10)
{

}

bool MouseEstimator::initialize()
{
    if(mouseEventTransmitter.initialize())
    {
        mouseEventTransmitter.moveToThread(&mouseEventThread);
        connect(&mouseEventThread,SIGNAL(started()),&mouseEventTransmitter,SLOT(update()));
        connect(&mouseEventTransmitter,SIGNAL(mouseChanged(int,int,unsigned long,bool)),this,SLOT(updateMouse(int,int,unsigned long,bool)));
        mouseEventThread.start();
        simulationTime.start();
        return true;
    } else
        return false;
}

void MouseEstimator::resetCalibration() {
    accumulatedVal = QVector2D(0,0);
}

void MouseEstimator::setCalibration(float distance) {
    mpd = QVector2D(distance/accumulatedVal.x(),distance/accumulatedVal.y());
    qDebug() << "Meter per MouseDot: " << mpd;
}

void MouseEstimator::updateMouse(int x, int y, unsigned long time, bool button) {
    QVector2D accelerationBuffer = QVector2D(x,-y);

    accumulatedVal += accelerationBuffer;
    mousePosition += accelerationBuffer;
    this->button = button;
}

void MouseEstimator::update() {
    double dT = simulationTime.nsecsElapsed()*1e-9;
    simulationTime.restart();

    QVector2D acceleration = massInverse*((-stiffness)*(position-mousePosition) - damping*velocity);
    velocity += dT*acceleration;
    position += dT*velocity;
}
