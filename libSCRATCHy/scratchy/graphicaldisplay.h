#ifndef GRAPHICALDISPLAY_H
#define GRAPHICALDISPLAY_H

#include <string>

class DisplayDetachable;
class ExternalEventLoop;

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

class GraphicalDisplay
{
public:
    GraphicalDisplay();
    ~GraphicalDisplay();

    bool detach();
    bool isPressed(Button button);

    void clear();
    void show(Icon icon, const std::string& header, const std::string& body);
    void show(Icon icon, const std::string& header, float body);
    void text(const std::string& text);

protected:
    DisplayDetachable* impl = nullptr;
    ExternalEventLoop* thread = nullptr;
};


#endif // GRAPHICALDISPLAY_H
