#include "signalmanager.h"

#include "lowlevel/iowrap.h"
#include <QDebug>

#include <unistd.h>

#include "graphicaldisplay.h"
#include "signalgenerator.h"

namespace SignalManagerInt
{
    bool i2cSendCommand(unsigned char targetAddress, SystemRequest command, unsigned int payload)
    {
        return IO::I2CSetAddress(targetAddress) &&
               IO::I2CWriteBlock(static_cast<int>(command), 4, reinterpret_cast<const unsigned char*>(&payload));
    }

    bool i2cQueryCommand(unsigned char targetAddress, SystemRequest command, unsigned int& payload)
    {
        return IO::I2CSetAddress(targetAddress) &&
               IO::I2CReadBlock(static_cast<int>(command), 4, reinterpret_cast<unsigned char*>(&payload));
    }
}

SignalManager::SignalManager()
{
    IO::GPIOInit();
    IO::SPIInit();
    IO::I2CInit();
    IO::GPIOSetBroadCast(false);
}

SignalManager::~SignalManager()
{
    IO::I2CFree();
    IO::SPIFree();
    IO::GPIOFree();

    for(SignalGenerator* g : generatorMap.values())
        delete g;
}


bool SignalManager::initializeBoards(GraphicalDisplay* display, unsigned int dacResolution)
{
    const unsigned long defaultWait = 100000;

    if(display) display->setDisplay(GraphicalDisplay::Icon::Pulse, "SignalBoards", QString("Initialization"));
    qDebug() << "Available I2C devices:" << scanDevices();
    if(display) display->setDisplay(GraphicalDisplay::Icon::Pulse, "SignalBoard", "Reset");
    reset();
    sleep(1);

    if(display) display->setDisplay(GraphicalDisplay::Icon::Pulse, "SignalBoard", "Initialization");
    initSystem();
    usleep(defaultWait);

    if(display) display->setDisplay(GraphicalDisplay::Icon::Pulse, "SignalBoard", "Device scan");
    assignAddresses();
    usleep(defaultWait);
    qDebug() << "Available I2C devices:" << scanDevices();


    if(display) display->setDisplay(GraphicalDisplay::Icon::Pulse, "SignalBoard", "r0 -> r1");
    for(SignalGenerator* g : generators())
        g->finishR0();
    sleep(1);

    if(display) display->setDisplay(GraphicalDisplay::Icon::Bolt, "SignalBoard", "SPI check");
    gatherSPISpeed();
    usleep(defaultWait);

    if(display) display->setDisplay(GraphicalDisplay::Icon::Pulse, "SignalBoard", "r1 -> r2");
    for(SignalGenerator* g : generators())
        g->finishR1();
    usleep(defaultWait);

    if(display) display->setDisplay(GraphicalDisplay::Icon::Pulse, "SignalBoard", "r2 -> r3");
    for(SignalGenerator* g : generators())
    {
        //g->setPanicMask(0b0000);
        //g->setSamplingRate(samplingRate); // Use board defaults
        g->setDACResolution(dacResolution);
        usleep(1e6);
        g->finishR2();
    }
    usleep(defaultWait);

    if(display) display->setDisplay(GraphicalDisplay::Icon::Pulse, "SignalBoard", "Start loop");
    for(SignalGenerator* g : generators())
        g->startSignalGeneration();
    usleep(defaultWait);

    // TODO: Wait till all boards reached r3 und return false after a certain time

    return true;
}


void SignalManager::maskDevice(uint8_t address)
{
    maskedDevices << address;
    qDebug() << "[I2C] Masked device" << address;
}

QVector<uint8_t> SignalManager::scanDevices()
{
    QVector<uint8_t> found;

    for(int n = 1; n < 255; n++)
    {
        //qDebug() << "[I2C] Scan port:" << n;
        if(!maskedDevices.contains(n) &&
                SignalManagerInt::i2cSendCommand(n, SystemRequest::Alive, 0))
        {
            //qDebug() << "Found I2C device:" << n;
            found << n;
        }
    }

    return found;
}

void SignalManager::reset(uint8_t address)
{
    qDebug() << "Resetting device no." << address;
    if(!SignalManagerInt::i2cSendCommand(address, SystemRequest::Reset, 0))
        qDebug() << "ERROR: Device did not respond!";
}

void SignalManager::reset()
{
    qDebug() << "Global reset initiated!";
    for(int n = 0; n < 255; n++)
    {
        bool found = !maskedDevices.contains(n) &&
                SignalManagerInt::i2cSendCommand(n, SystemRequest::Reset, 0);
        if(found)
            qDebug() << "Device no." << n << " resetted";
    }
    qDebug() << "Global reset done.";
}

void SignalManager::initSystem()
{
    SignalManagerInt::i2cSendCommand(0x0, SystemRequest::InitSystem, 0);
    usleep(1000000);
}

void SignalManager::assignAddresses()
{
    qDebug() << "Clearing previous mapping...";
    for(SignalGenerator* gen : generatorMap)
        delete gen;
    generatorMap.clear();

    qDebug() << "Assigning I2C addresses...";

    for(int n = 1; n < 255; n++)
    {
        if(maskedDevices.contains(n)) continue;
        IO::GPIOSetAddress(n);
        SignalManagerInt::i2cSendCommand(0x0, SystemRequest::SetI2CAddress, n << (3 * 8));

        usleep(1000);

        if(SignalManagerInt::i2cSendCommand(n, SystemRequest::Alive, 0) )
        {
            qDebug() << "Assigned address no." << n;
            generatorMap[n] = new SignalGenerator(n);
        }
    }

    IO::GPIOSetAddress(0);
}

void SignalManager::gatherSPISpeed()
{
    // This is currently under construction
    // There seems to be some kind of resetting error, causing the teensy to read somewhat "shifted" data
    /*
    qDebug() << "Determining maximum SPI speed...";
    generators()[0]->spiCheck(1024*16);
    generators()[0]->spiCheck(64);
    generators()[0]->spiCheck(512);
    */
}

QList<SignalGenerator*> SignalManager::generators()
{
    return generatorMap.values();
}

QList<uint8_t> SignalManager::addresses()
{
    return generatorMap.keys();
}

SignalGenerator* SignalManager::generator(uint8_t address)
{
    return generatorMap[address];
}
