#ifndef MOUSEPOSITIONQUERY_H
#define MOUSEPOSITIONQUERY_H

#include <scratchy/positionquery.h>

class MousePositionQuery : public PositionQuery
{
public:
    MousePositionQuery();
    virtual ~MousePositionQuery();

    // Implementations of interface PositionQuery:
    virtual QVector2D position() const;
    virtual QVector2D velocity() const;
    virtual float orientation() const;
    virtual float angularVelocity() const;
    virtual bool buttonPressed() const;

    virtual void update();
    virtual bool initialize();

private:
    class impl;
    impl* implementation;
};

#endif // MOUSEPOSITIONQUERY_H
