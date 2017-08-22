#ifndef GRAPHICALDISPLAY_H
#define GRAPHICALDISPLAY_H

#include <QObject>
#include <QString>
#include <QPixmap>
#include <QPoint>
#include <QThread>

// Warning: BeagleBone uses /dev/i2c-2 instead of /dev/i2c-1
class DisplayDetachable;

class GraphicalDisplay
{
public:
    GraphicalDisplay();
    ~GraphicalDisplay();
    bool detach();

    enum Icon
    {
        None,
        wlLogo,
        Logo,
        Spectrum,
        Forbidden,
        Bolt,
        Bug,
        Check,
        Clock,
        Cog,
        Fire,
        Image,
        Reboot,
        Power,
        Pulse,
        Random,
        Warning,
        Wrench,
        X
    };

    enum Button
    {
        // BeagleBone
        // Back = 11,
        // Up = 12,
        // Down = 13,
        // Select = 26

        // Raspberry
        Back = 4,
        Up = 6,
        Down = 5,
        Select = 1
    };

    enum FontSize
    {
        Small, Medium, Big
    };

    bool isPressed(Button button);

    void clear();
    void setDisplay(Icon icon, const QString& header, const QString& body);
    void setText(const QString& text);

    void errorShutdown(QString message, unsigned int timeout = 5);

signals:

protected:
    DisplayDetachable* impl = nullptr;

    class ExternalEventLoop : public QThread
    {
    protected:
        void run() { exec(); }
    };

    ExternalEventLoop* thread = nullptr;
};

class DisplayDetachable : public QObject
{
    Q_OBJECT

public:
    DisplayDetachable();
    virtual ~DisplayDetachable();

signals:
    void finished();

public slots:
    void clear();
    void setDisplay(int icon, QString header, QString body);
    void setText(const QString& text);

protected:
    void setIcon(GraphicalDisplay::Icon icon);
    void drawText(const QString& text, unsigned int offset, unsigned int xOffset, GraphicalDisplay::FontSize size);

    bool initialized = false;
};

#endif // GRAPHICALDISPLAY_H
