#include "mousepositionquery.h"
#include "positionproviders/mouseeventtransmitter.h"
#include <QDebug>
#include <QElapsedTimer>
#include <QThread>

class MousePositionQuery::impl
{
private:
    MouseEventTransmitter mouseEventTransmitter;
    QThread mouseEventThread;
    QVector2D mousePosition;
    QVector2D position;
    QVector2D velocity;
    QVector2D accumulatedVal;
    QElapsedTimer simulationTime;
    QVector2D mpd;
    double massInverse;
    double stiffness;
    double damping;
    bool button = false;

public:
    impl():
        mpd(0.1/4480,0.1/4378),
        massInverse(100),
        stiffness(500),
        damping(10)
    {

    }

    QVector2D getPosition() const
    {
        return position * mpd;
    }

    QVector2D getVelocity() const
    {
        return velocity * mpd;
    }

    bool getButton() const
    {
        return button;
    }

    bool initialize()
    {
        if(mouseEventTransmitter.initialize())
        {
            mouseEventTransmitter.moveToThread(&mouseEventThread);
            QObject::connect(&mouseEventThread, &QThread::started,
                             &mouseEventTransmitter, &MouseEventTransmitter::update);
            QObject::connect(&mouseEventTransmitter, &MouseEventTransmitter::mouseChanged,
                             [&](int x, int y, bool button) {
                                this->updateMouse(x, y, button);
                             });
            mouseEventThread.start();
            simulationTime.start();
            return true;
        } else
            return false;
    }

    void update()
    {
        double dT = simulationTime.nsecsElapsed()*1e-9;
        simulationTime.restart();

        QVector2D acceleration = massInverse*((-stiffness)*(position-mousePosition) - damping*velocity);
        velocity += dT*acceleration;
        position += dT*velocity;
    }

    void updateMouse(int x, int y, bool button)
    {
        QVector2D accelerationBuffer = QVector2D(x,-y);

        accumulatedVal += accelerationBuffer;
        mousePosition += accelerationBuffer;
        this->button = button;
    }

    void resetCalibration()
    {
        accumulatedVal = QVector2D(0,0);
    }

    void setCalibration(float distance=0.1)
    {
        mpd = QVector2D(distance/accumulatedVal.x(),distance/accumulatedVal.y());
        qDebug() << "Meter per MouseDot: " << mpd;
    }
};

MousePositionQuery::MousePositionQuery()
{
    implementation = new impl;
}

MousePositionQuery::~MousePositionQuery()
{
    delete implementation;
}

bool MousePositionQuery::initialize()
{
    return implementation->initialize();
}

void MousePositionQuery::update()
{
    implementation->update();
}

QVector2D MousePositionQuery::position() const
{
    return implementation->getPosition();
}

QVector2D MousePositionQuery::velocity() const
{
    return implementation->getVelocity();
}

float MousePositionQuery::orientation() const
{
    return 0.0;
}

float MousePositionQuery::angularVelocity() const
{
    return 0.0;
}

bool MousePositionQuery::buttonPressed() const
{
    return implementation->getButton();
}
