#ifndef MOUSEESTIMATOR_H
#define MOUSEESTIMATOR_H

#include <QVector2D>
#include <QObject>
#include <QThread>
#include <QElapsedTimer>
#include <QDebug>
#include "positionproviders/mouseeventtransmitter.h"

class MouseEstimator : public QObject
{
    Q_OBJECT
public:
    MouseEstimator();

public slots:

    QVector2D getPosition() const { return position*mpd; }
    QVector2D getVelocity() const { return velocity*mpd; }
    bool getButton() const { return button; }

    bool initialize();
    void update();
    void updateMouse(int x, int y, unsigned long time, bool button);
    void resetCalibration();
    void setCalibration(float distance=0.1);

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
};

#endif // MOUSEESTIMATOR_H
