import sys
import os
sys.path.append(os.path.abspath('./build/lib.linux-x86_64-3.6/'))
import SCRATCHPy as scratchy
import time

display = scratchy.GraphicalDisplay()
display.clear()
display.text("Hello, World!")
#time.sleep(5)
display.show(scratchy.GraphicalDisplay.Icon.wlLogo,"Hello","SCRATCHy")

generator = scratchy.SignalGenerator(10)
status = generator.status()
print('Status: {}'.format(status))

#time.sleep(5)
