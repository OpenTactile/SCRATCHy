#include "mousepositionquery.h"
#include "mouseestimator.h"
#include <QDebug>

struct MousePositionQuery::impl
{
    MouseEstimator estimator;
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
    return implementation->estimator.initialize();
}

void MousePositionQuery::update()
{
    implementation->estimator.update();
}

QVector2D MousePositionQuery::position() const
{
    return implementation->estimator.getPosition();
}

QVector2D MousePositionQuery::velocity() const
{
    return implementation->estimator.getVelocity();
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
    return implementation->estimator.getButton();
}
