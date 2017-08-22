#ifndef MOUSEEVENTTRANSMITTER_H
#define MOUSEEVENTTRANSMITTER_H

#include <QObject>

class MouseEventTransmitter : public QObject
{
    Q_OBJECT
public:
    explicit MouseEventTransmitter(QObject *parent = 0);
    bool initialize();

public slots:
    void update();

signals:
    void mouseChanged(int x, int y, bool button);

private:
    bool button = false;
};

#endif // MOUSEEVENTTRANSMITTER_H
