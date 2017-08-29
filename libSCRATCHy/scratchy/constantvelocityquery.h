#ifndef CONSTANTVELOCITYQUERY_H
#define CONSTANTVELOCITYQUERY_H

#include <scratchy/positionquery.h>

class ConstantVelocityQuery : public PositionQuery
{
public:
    ConstantVelocityQuery(float velocity = 0.05, float wrapAfter = 0.25);
    ~ConstantVelocityQuery();

    // Implementations of interface PositionQuery:
    virtual QVector2D position() const;
    virtual QVector2D velocity() const;
    virtual float orientation() const;
    virtual float angularVelocity() const;
    virtual bool buttonPressed() const {return false;}

    virtual bool initialize();
    virtual void update();

private:
    struct impl;
    impl* implementation;
};

#endif // CONSTANTVELOCITYQUERY_H
