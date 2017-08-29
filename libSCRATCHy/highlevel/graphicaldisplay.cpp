#include "scratchy/graphicaldisplay.h"

#include "scratchy/iowrap.h"
#include "highlevel/displaydetachable.h"

#include <unistd.h>
#include <QDebug>

GraphicalDisplay::GraphicalDisplay()
{
    IO::GPIOInit();

    IO::GPIOSetDirection(Button::Back, IO::input_pullup);
    IO::GPIOSetDirection(Button::Select, IO::input_pullup);
    IO::GPIOSetDirection(Button::Up, IO::input_pullup);
    IO::GPIOSetDirection(Button::Down, IO::input_pullup);

    impl = new DisplayDetachable();
}

GraphicalDisplay::~GraphicalDisplay()
{
    delete impl;
    if(thread)
    {
        thread->wait(100000);
        thread->terminate();
        delete thread;
    }
    IO::GPIOFree();
}

bool GraphicalDisplay::detach()
{
    if(thread)
        return true;

    thread = new ExternalEventLoop;
    if(!thread)
        return false;

    impl->moveToThread(thread);
    QObject::connect(impl, SIGNAL(finished()), thread, SLOT(quit()));
    QObject::connect(impl, SIGNAL(finished()), thread, SLOT(deleteLater()));
    // TODO: Check for proper destruction of impl!
    thread->start();
    return true;
}

void GraphicalDisplay::clear()
{    
    QMetaObject::invokeMethod(impl, "clear", thread? Qt::QueuedConnection : Qt::DirectConnection);
}

bool GraphicalDisplay::isPressed(Button button)
{
    return IO::GPIOIsLow(button);
}

void GraphicalDisplay::show(Icon icon, const QString& header, float body)
{
    show(icon, header, QString::number(body));
}

void GraphicalDisplay::show(Icon icon, const QString& header, const QString& body)
{    
    int ic = (int)icon;
    QMetaObject::invokeMethod(impl, "setDisplay", thread? Qt::QueuedConnection : Qt::DirectConnection,
                              Q_ARG(int, ic),
                              Q_ARG(QString, header),
                              Q_ARG(QString, body));
}


void GraphicalDisplay::text(const QString& text)
{
    QMetaObject::invokeMethod(impl, "setText", thread? Qt::QueuedConnection : Qt::DirectConnection,
                              Q_ARG(QString, text));

}
