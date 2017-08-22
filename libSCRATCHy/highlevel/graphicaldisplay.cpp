#include "graphicaldisplay.h"

#include "lowlevel/imagehandlingoled.h"
#include "lowlevel/oleddisplay.h"
#include <unistd.h>

#include <QPixmap>
#include <QPainter>
#include "external/rastafont/rastafont.h"
#include "lowlevel/iowrap.h"
#include <QThread>

#include <QDebug>
#include <QThread>


class ExternalEventLoop : public QThread
{
protected:
    void run() { exec(); }
};

class DisplayDetachable : public QObject
{
    Q_OBJECT

public:
    DisplayDetachable();
    virtual ~DisplayDetachable();

    enum FontSize
    {
        Small, Medium, Big
    };

signals:
    void finished();

public slots:
    void clear();
    void setDisplay(int icon, QString header, QString body);
    void setText(const QString& text);

protected:
    void setIcon(GraphicalDisplay::Icon icon);
    void drawText(const QString& text, unsigned int offset, unsigned int xOffset, FontSize size);

    bool initialized = false;
};

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

void GraphicalDisplay::setDisplay(Icon icon, const QString& header, const QString& body)
{    
    int ic = (int)icon;
    QMetaObject::invokeMethod(impl, "setDisplay", thread? Qt::QueuedConnection : Qt::DirectConnection,
                              Q_ARG(int, ic),
                              Q_ARG(QString, header),
                              Q_ARG(QString, body));
}


void GraphicalDisplay::setText(const QString& text)
{
    QMetaObject::invokeMethod(impl, "setText", thread? Qt::QueuedConnection : Qt::DirectConnection,
                              Q_ARG(QString, text));

}


void GraphicalDisplay::errorShutdown(QString message, unsigned int timeout)
{
    for(unsigned int n = timeout; n > 0; n--)
    {
        setDisplay(Icon::Warning, "Error", message + "\nShutdown in " + QString::number(n));
        usleep(1000000);
    }

    setDisplay(Icon::None, "", "");
    exit(-1);
}


// Actual implementation, separated to allow easier multithreading via QThread

DisplayDetachable::DisplayDetachable()
{
    ImageHandlingOLED::init();
    OLEDDisplay::set_contrast(0);
    initialized = true;
}

DisplayDetachable::~DisplayDetachable()
{
    ImageHandlingOLED::free();
    emit finished();
}

void DisplayDetachable::clear()
{
    if(initialized)
        ImageHandlingOLED::clear(0);
    usleep(100);
}

void DisplayDetachable::setDisplay(int icon, QString header, QString body)
{
    if(!initialized) return;
    clear();
    setIcon((GraphicalDisplay::Icon)icon);
    drawText(header, 2, 35, DisplayDetachable::FontSize::Big);     // 8x12 pixel font
    drawText(body,  16, 35, DisplayDetachable::FontSize::Small);   // 6x 8 pixel font
    ImageHandlingOLED::display();
    usleep(100);
}


void DisplayDetachable::setText(const QString& text)
{
    if(!initialized) return;
    clear();

    QString txt;
    int availableCharacters = 128 / 6;
    int cnt = 0;
    for(int n = 0; n < text.size(); n++)
    {
        txt += text.at(n);
        if(text.at(n) == '\n')
        {
            cnt = 0;
        }
        else
        {
            cnt++;
        }

        if(cnt >= availableCharacters)
        {
            txt += "\n";
            cnt = 0;
        }
    }


    drawText(txt, 0, 0, DisplayDetachable::FontSize::Small);   // 6x 8 pixel font
    ImageHandlingOLED::display();
    usleep(100);
}


void DisplayDetachable::setIcon(GraphicalDisplay::Icon icon)
{
    if(!initialized) return;

    static QString resourceNames[19] = {
        "none", "scratchy", "logo", "spectrum", "forbidden", "bolt", "bug", "check",
        "clock", "cog", "fire", "image", "reboot", "power", "pulse", "random",
        "warning", "wrench", "x"
    };

    QString resourceName = QString(":/images/images/") +
            resourceNames[static_cast<unsigned int>(icon)] +
            QString(".png");

    QImage img = QImage(resourceName).convertToFormat(QImage::Format_Mono, Qt::MonoOnly);
    QPainter p(&ImageHandlingOLED::actlImage);
    p.drawImage(QRect(0, 0, 32, 32), img);
    p.end();
    usleep(100);
}


void DisplayDetachable::drawText(const QString& text, unsigned int offset, unsigned int xOffset, DisplayDetachable::FontSize size)
{
    if(!initialized) return;
    // TODO: Allow animation of long lines (scrolling text)!

    const unsigned int availablePixels = 128 - xOffset;
    unsigned int fontWidth = ((size == DisplayDetachable::FontSize::Small)? 6 : 8);
    unsigned int fontHeight = ((size == DisplayDetachable::FontSize::Big)? 12 : 8);
    int availableCharactersX = availablePixels / fontWidth;
    int availableCharactersY = (32 - offset) / fontHeight;

    QStringList lines = text.split('\n');

    while(lines.size() > availableCharactersY)
        lines.removeLast();

    unsigned int yOffset = offset;
    for(const QString& line : lines)
    {
        QString ln = line.toLocal8Bit().leftJustified(
                    availableCharactersX, ' ', true);
        std::string currentLine = ln.toStdString();

        QImage renderBuffer(QSize(availablePixels, fontHeight*lines.size()), QImage::Format_RGBA8888);
        renderBuffer.fill(0);

        switch(size)
        {
        case FontSize::Small:
            rastafont6x8_blit_string(reinterpret_cast<unsigned int*>(renderBuffer.bits()),
                                     availablePixels * sizeof(unsigned int),
                                     0xFFFFFFFF, currentLine.c_str());
            break;

        case FontSize::Medium:
            rastafont8x8_blit_string(reinterpret_cast<unsigned int*>(renderBuffer.bits()),
                                     availablePixels * sizeof(unsigned int),
                                     0xFFFFFFFF, currentLine.c_str());
            break;

        case FontSize::Big:
            rastafont8x12_blit_string(reinterpret_cast<unsigned int*>(renderBuffer.bits()),
                                      availablePixels * sizeof(unsigned int),
                                      0xFFFFFFFF, currentLine.c_str());
            break;
        }

        QPainter p(&ImageHandlingOLED::actlImage);
        p.drawImage(xOffset, yOffset, renderBuffer);
        p.end();

        yOffset += fontHeight;
    }
    usleep(100);
}
