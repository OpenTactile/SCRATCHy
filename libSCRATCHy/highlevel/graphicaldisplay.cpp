#include "scratchy/graphicaldisplay.h"

#include "scratchy/iowrap.h"
#include "highlevel/displaydetachable.h"

#include <unistd.h>
#include <QDebug>
#include <sstream>

/*
#include <QObject>
#include <QString>
#include <QPixmap>
#include <QPoint>
*/

GraphicalDisplay::GraphicalDisplay()
{
    IO::GPIOInit();

    IO::GPIOSetDirection(Button::Back, IO::input_pullup);
    IO::GPIOSetDirection(Button::Select, IO::input_pullup);
    IO::GPIOSetDirection(Button::Up, IO::input_pullup);
    IO::GPIOSetDirection(Button::Down, IO::input_pullup);

    qRegisterMetaType<std::string>("std::string");

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

void GraphicalDisplay::show(Icon icon, const std::string& header, float body)
{
    std::ostringstream ss;
    ss << body;
    show(icon, header, ss.str());
}

void GraphicalDisplay::show(Icon icon, const std::string& header, const std::string& body)
{    
    int ic = static_cast<int>(icon);
    QMetaObject::invokeMethod(impl, "setDisplay", thread? Qt::QueuedConnection : Qt::DirectConnection,
                              Q_ARG(int, ic),
                              Q_ARG(std::string, header),
                              Q_ARG(std::string, body));
}


void GraphicalDisplay::text(const std::string& text)
{
    QMetaObject::invokeMethod(impl, "setText", thread? Qt::QueuedConnection : Qt::DirectConnection,
                              Q_ARG(std::string, text));

}
