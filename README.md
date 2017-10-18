# SCRATCHy  
The *SCalable Reference Architecture for a Tactile display Control Hardware*

SCRATCHy is an open hardware- and software-plattform for controlling so-called "tactile Displays", i. e. devices that are intended to stimulate the touch sensitive receptors in human skin. In combination with [ITCHy](https://github.com/OpenTactile/ITCHy), the tactile mouse, this project forms a complete research environment for e. g. developing new models controlling tactile displays or for conducting user studies.

This repository contains the following parts that reflect the folder structure:
  * `./pcb/` &rarr; Schematics and layouts for printed circuit boards
  * `./libSCRATCHy/` &rarr; Library to access the Signal Generators and peripherals
  * `./SCRATCHPy/` &rarr; Python wrapper covering the high-level functionality of libSCRATCHy
  * `./examplesCpp/` &rarr; Example programs using the C++ interface
  * `./examplesPython/` &rarr; Example programs using the Python interface

There are still some parts missing that are currently under development and will be available soon:
  * Signal Generator firmware
  * *ScratchyShow* graphical interface to libSCRATCHy ([separate respository](https://github.com/OpenTactile/ScratchyShow))
  * [Raspberry Pi 3](https://www.raspberrypi.org/products/raspberry-pi-3-model-b/) compatible SD Card image (will be hosted in separate repository due to its size)

## Assembling the hardware system
The circuit boards located in the `./pcb/` folder have been designed with [Fritzing](http://fritzing.org/home/), an open source EDA software. For convenience, production ready [Extended Gerber](https://en.wikipedia.org/wiki/Gerber_format) files can be found in their respective subfolders.

Since most of the parts on the PCBs come in SMD packages, some experience in soldering such small parts is beneficial. However, the smallest components used here (0805 Package) can still be soldered by hand easily. (There are some nice [tutorial videos](https://www.youtube.com/watch?v=b9FC9fAlfQE) on how to solder SMD components by hand.)

Each of the PCB subfolders contains a Bill Of Material (BOM) listing the components required for a single board. The part numbers given in these lists can be looked up on [Farnell/Newark](http://www.farnell.com/).

#### System overview
![Image of Yaktocat](documentation/images/hardware.jpg)
The hardware system consists of a single main processor and several so-called *Signal Generators* (SGs) that are connected to a custom SPI/I²C bus. Each of these SGs is able to generate up to 4 indepentent analog signals. Depending of the type of SG, these signals can be amplified externally or are already amplified to up to &plusmn;200 V (theoretically, has been tested with 120V).

###### MainComputeUnit
*ToDo*

###### DisplayBreakout
*ToDo*

###### AnalogSignalGenerator
*ToDo*

###### HighVoltageAmplifier
*ToDo*
**Do not forget a disclaimer regarding the high voltages. Use at own risk!**

###### PiezoFilter
*ToDo*


## Programming the Signal Generators

## Building and installing libraries & dependencies

## First steps in using libSCRATCHy

## Interfacing with Python

# Contributing

# Authors

# License
This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details

#Acknowledgments