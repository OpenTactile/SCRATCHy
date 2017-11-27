#ifndef POSITIONQUERY_H
#define POSITIONQUERY_H

#include <QVector2D>

/* interface */ class PositionQuery
{
public:
    virtual ~PositionQuery() {}

    // Absolute position in meters
    virtual QVector2D position() const = 0;

    // Current velocity in meters/second
    virtual QVector2D velocity() const = 0;

    virtual float orientation() const = 0;
    virtual float angularVelocity() const = 0;

    virtual bool buttonPressed() const = 0;

    virtual bool initialize() = 0;

    virtual void update() = 0;

    virtual void feedback(unsigned char r, unsigned char g, unsigned char b)
    {
        Q_UNUSED(r); Q_UNUSED(g); Q_UNUSED(b);
    }

};

#endif // POSITIONQUERY_H

