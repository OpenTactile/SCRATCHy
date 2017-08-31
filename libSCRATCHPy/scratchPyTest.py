import sys
import os
sys.path.append(os.path.abspath('./build/lib.linux-x86_64-3.6/'))
import SCRATCHPy as scratchy
import time

display = scratchy.GraphicalDisplay()
display.clear()
display.text("Hello, World!")
#time.sleep(5)
display.show(scratchy.Icon.wlLogo,"Hello","SCRATCHy")
print('Button Back pressed: {}'.format(display.isPressed(scratchy.Button.Back)))

q15 = scratchy.fixed_q15(0.5)
print('Q15: {}'.format(q15))
q15.value = 1.5
print('Q15: {}'.format(q15))

q5 = scratchy.fixed_q5(1000)
print('Q5: {}'.format(q5))
q5.value = 2000
print('Q5: {}'.format(q5))

arrQ5 = scratchy.Array10Q5()
arrQ5[5].value = 500
print('arrQ5[5]: {}'.format(arrQ5[5]))

table = scratchy.FrequencyTable()
table.frequency[5].value = 400
table.amplitude[5].value = .75
print('Freq: {}'.format(table.frequency[5]))
print('Amp: {}'.format(table.amplitude[5]))

tableArray = scratchy.FrequencyTableArray()
tableArray[2].frequency[6].value = 500
tableArray[2].amplitude[6].value = 0.85
print('Freq: {}'.format(tableArray[2].frequency[6]))
print('Amp: {}'.format(tableArray[2].amplitude[6]))

generator = scratchy.SignalGenerator(10)
print('Generator: {}'.format(generator))
status = generator.status()
print('Status: {}'.format(status))

generator.send(table, table, table, table)
generator.send(table)
generator.send(tableArray)

generators = scratchy.SignalGeneratorVector()
generators.append(generator)
generators.append(generator)
print('Generators: {}'.format(generators))
print('Generator[0]: {}'.format(generators[0]))


manager = scratchy.SignalManager()
boards = manager.scanDevices()
print('Boards: {}'.format(boards))

generators = manager.generators()
print('Generators: {}'.format(generators))

#time.sleep(5)
