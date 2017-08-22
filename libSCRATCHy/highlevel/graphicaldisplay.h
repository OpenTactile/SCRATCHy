#ifndef GRAPHICALDISPLAY_H
#define GRAPHICALDISPLAY_H

#include <QObject>
#include <QString>
#include <QPixmap>
#include <QPoint>

class DisplayDetachable;
class ExternalEventLoop;

class GraphicalDisplay
{
public:    
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
        // Raspberry Pi 3
        Back = 4,
        Up = 6,
        Down = 5,
        Select = 1
    };

    GraphicalDisplay();
    ~GraphicalDisplay();

    bool detach();
    bool isPressed(Button button);

    void clear();
    void setDisplay(Icon icon, const QString& header, const QString& body);
    void setText(const QString& text);

    void errorShutdown(QString message, unsigned int timeout = 5);

protected:
    DisplayDetachable* impl = nullptr;
    ExternalEventLoop* thread = nullptr;
};


#endif // GRAPHICALDISPLAY_H
