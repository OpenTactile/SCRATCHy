import sys
import os
#TODO: make independet from build path
sys.path.append(os.path.abspath('../../SCRATCHPy/build/lib.linux-x86_64-3.6/'))
sys.path.append(os.path.abspath('../../../ITCHy/ITCHPy/build/lib.linux-x86_64-3.6/'))

import SCRATCHPy
import ITCHPy

if __name__ == '__main__':
    display = SCRATCHPy.GraphicalDisplay()
    display.show(SCRATCHPy.Icon.Logo, 
                 'Demo', 'Minimum example')
    display.detach()

    data = SCRATCHPy.FrequencyTable()
    itchy = ITCHPy.TactileMouseQuery()
    itchy.initialize()
    controller = SCRATCHPy.SignalManager()
    controller.initializeBoards()

    while True:
        itchy.update()
        velocity = itchy.velocity().length();

        # Scale frequency with movement speed:
        data.frequency[0].value = velocity * 25.0;
        data.amplitude[0].value = 1.0;

        for g in controller.generators():
            g.send(data);

        display.show(SCRATCHPy.Icon.Logo, 'Frequency',
                     data.frequency[0]);

        # Exit loop if button is pressed
        if display.isPressed(SCRATCHPy.Button.Back):
            break