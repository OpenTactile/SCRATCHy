import sys
import os
#TODO: make independet from build path
sys.path.append(os.path.abspath('./build/lib.linux-x86_64-3.6/'))
import SCRATCHPy as scratchy
import time

import unittest

class TestDisplay(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.display = scratchy.GraphicalDisplay()

    def test_display(self):
        interactive_wait = 0
        self.display.clear()
        self.display.text("Hello, World!")
        time.sleep(interactive_wait)
        self.display.show(scratchy.Icon.wlLogo,"Hello","SCRATCHy")
        time.sleep(interactive_wait)
        self.display.show(scratchy.Icon.Pulse,"Hello",12.5)

    def test_detach(self):
        self.display.detach()

    def test_buttons(self):
        self.assertEqual(self.display.isPressed(scratchy.Button.Back), False)
        self.assertEqual(self.display.isPressed(scratchy.Button.Up), False)
        self.assertEqual(self.display.isPressed(scratchy.Button.Down), False)
        self.assertEqual(self.display.isPressed(scratchy.Button.Select), False)

class TestFrequencyTable(unittest.TestCase):

    def test_q15(self):
        q15 = scratchy.fixed_q15(0.5)
        self.assertAlmostEqual(q15.value, 0.5)
        q15.value = 1.5
        self.assertAlmostEqual(q15.value, 1.5)

    def test_q5(self):
        q5 = scratchy.fixed_q5(1000)
        self.assertAlmostEqual(q5.value, 1000)
        q5.value = 2000
        self.assertAlmostEqual(q5.value, 2000)

    def test_q15_array(self):
        arrQ15 = scratchy.Array10Q15()
        arrQ15[5].value = 0.75
        self.assertAlmostEqual(arrQ15[0].value, 0)
        self.assertAlmostEqual(arrQ15[5].value, 0.75)
        self.assertAlmostEqual(arrQ15[9].value, 0)
        with self.assertRaises(SystemError):
            arrQ15[10].value = 10

    def test_q5_array(self):
        arrQ5 = scratchy.Array10Q5()
        arrQ5[5].value = 500
        self.assertAlmostEqual(arrQ5[0].value, 0)
        self.assertAlmostEqual(arrQ5[5].value, 500)
        self.assertAlmostEqual(arrQ5[9].value, 0)
        with self.assertRaises(SystemError):
            arrQ5[10].value = 10

    def test_frequencytable(self):
        table = scratchy.FrequencyTable()
        table.frequency[5].value = 400
        table.amplitude[5].value = .75
        self.assertAlmostEqual(table.frequency[0].value, 0)
        self.assertAlmostEqual(table.amplitude[0].value, 0)
        self.assertAlmostEqual(table.frequency[5].value, 400)
        self.assertAlmostEqual(table.amplitude[5].value, 0.75)
        self.assertAlmostEqual(table.frequency[9].value, 0)
        self.assertAlmostEqual(table.amplitude[9].value, 0)

    def test_frequencytable_array(self):
        tableArray = scratchy.FrequencyTableArray()
        tableArray[2].frequency[5].value = 400
        tableArray[2].amplitude[5].value = .75
        self.assertAlmostEqual(tableArray[2].frequency[0].value, 0)
        self.assertAlmostEqual(tableArray[2].amplitude[0].value, 0)
        self.assertAlmostEqual(tableArray[2].frequency[5].value, 400)
        self.assertAlmostEqual(tableArray[2].amplitude[5].value, 0.75)
        self.assertAlmostEqual(tableArray[2].frequency[9].value, 0)
        self.assertAlmostEqual(tableArray[2].amplitude[9].value, 0)

class TestSignalGenerator(unittest.TestCase):

    def test_creation(self):
        generator = scratchy.SignalGenerator(10)
        self.assertEqual(generator.address(), 10)

    def test_status(self):
        generator = scratchy.SignalGenerator(10)
        self.assertEqual(generator.status(), scratchy.SystemStatus.NotResponding)

    def test_send(self):
        generator = scratchy.SignalGenerator(10)

        table = scratchy.FrequencyTable()
        table.frequency[5].value = 400
        table.amplitude[5].value = .75

        generator.send(table)
        generator.send(table, table, table, table)

        tableArray = scratchy.FrequencyTableArray()
        tableArray[2].frequency[5].value = 400
        tableArray[2].amplitude[5].value = .75

        generator.send(tableArray)

    def test_vector(self):
        generators = scratchy.SignalGeneratorVector()
        generators.append(scratchy.SignalGenerator(10))
        generators.append(scratchy.SignalGenerator(20))

        self.assertEqual(generators[0].address(), 10)
        self.assertEqual(generators[1].address(), 20)

class TestSignalManager(unittest.TestCase):

    @classmethod
    def setUpClass(cls):
        cls.manager = scratchy.SignalManager()

    #TODO: Only working when no boards are connected, how to test?
    def test_scan(self):
        boards = self.manager.scanDevices()
        self.assertEqual(len(boards), 0)

    #TODO: Only working when no boards are connected, how to test?
    def test_generators(self):
        generators = self.manager.generators()
        self.assertEqual(len(generators), 0)

if __name__ == '__main__':
    unittest.main(verbosity=2)
