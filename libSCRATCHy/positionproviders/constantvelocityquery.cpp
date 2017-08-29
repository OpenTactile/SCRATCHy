#include "scratchy/constantvelocityquery.h"
#include <QTime>
#include <cmath>
#include <QElapsedTimer>


struct ConstantVelocityQuery::impl
{
    float velocity;
    float xWrap;
    QElapsedTimer timer;
};

ConstantVelocityQuery::ConstantVelocityQuery(float velocity, float wrapAfter)
{
    implementation = new impl;
    implementation->velocity = velocity;
    implementation->xWrap = wrapAfter;
    implementation->timer.start();
}

ConstantVelocityQuery::~ConstantVelocityQuery()
{
    delete implementation;
}

QVector2D ConstantVelocityQuery::position() const
{
    float timestamp = implementation->timer.nsecsElapsed()*1.0e-9;
    float position = timestamp * velocity().x();
    return QVector2D(fmod(position, implementation->xWrap), 0.0);
}

QVector2D ConstantVelocityQuery::velocity() const
{
    return QVector2D(implementation->velocity, 0.0);
}

float ConstantVelocityQuery::orientation() const
{
    return 0.0;
}

float ConstantVelocityQuery::angularVelocity() const
{
    return 0.0;
}

bool ConstantVelocityQuery::initialize()
{
    return true;
}

void ConstantVelocityQuery::update()
{

}
