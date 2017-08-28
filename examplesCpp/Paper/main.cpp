#include <scratchy>

int main(int argc, char *argv[]) {
    GraphicalDisplay display;
    display.setDisplay(GraphicalDisplay::Icon::Logo,
                       "Demo", "Minimum example");
    display.detach();

    FrequencyTable data;
    ConstantVelocityQuery itchy;
    SignalManager controller;
    controller.initializeBoards();

    while(true) {
        itchy.update();
        float velocity = itchy.velocity().length();

        // Scale frequency with movement speed:
        data.frequency[0] = velocity * 25.0;
        data.amplitude[0] = 1.0;

        for(SignalGenerator& g: controller.generators())
            g.sendTables(data);

        display.setDisplay(GraphicalDisplay::Icon::Logo, "Frequency",
                           QString::number(data.frequency[0]));

        // Exit loop if button is pressed
        if(display.isPressed(GraphicalDisplay::Button::Back))
            break;
    }

    return 0;
}
