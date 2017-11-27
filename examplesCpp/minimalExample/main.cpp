#include <scratchy>
#include <itchy>

int main(int argc, char *argv[]) {
    GraphicalDisplay display;
    display.show(Icon::Logo,
                 "Demo", "Minimum example");
    display.detach();

    FrequencyTable data;
    TactileMouseQuery itchy;
    SignalManager controller;
    controller.initializeBoards();

    while(true) {
        itchy.update();
        float velocity = itchy.velocity().length();

        // Scale frequency with movement speed:
        data.frequency[0] = velocity * 25.0;
        data.amplitude[0] = 1.0;

        for(SignalGenerator& g: controller.generators())
            g.send(data);

        display.show(Icon::Logo, "Frequency",
                     data.frequency[0]);

        // Exit loop if button is pressed
        if(display.isPressed(Button::Back))
            break;
    }

    return 0;
}
