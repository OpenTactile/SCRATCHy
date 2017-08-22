#include "signalmanager.h"

#include <QDebug>
#include <unistd.h>
#include "lowlevel/iowrap.h"
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

    generatorList.clear();
}


bool SignalManager::initializeBoards(unsigned int dacResolution, unsigned int samplingTime)
{
    const unsigned long defaultWait = 100000;
    qDebug() << "Initializing Signal Boards";
    qDebug() << "\tAvailable I2C devices:" << scanDevices();
    qDebug() << "\tReset";
    reset();
    sleep(1);

    qDebug() << "\tInitialization";
    initSystem();
    usleep(defaultWait);

    qDebug() << "\tDevice scan";
    assignAddresses();
    usleep(defaultWait);
    qDebug() << "\tAvailable I2C devices:" << scanDevices();

    qDebug() << "\tr0 -> r1";
    for(SignalGenerator& g : generators())
        g.finishR0();
    sleep(1);

    qDebug() << "\tSPI check";
    gatherSPISpeed();
    usleep(defaultWait);

    qDebug() << "\tr1 -> r2";
    for(SignalGenerator& g : generators())
        g.finishR1();
    usleep(defaultWait);

    qDebug() << "\tr2 -> r3";
    for(SignalGenerator& g : generators())
    {        
        g.setSamplingRate(samplingTime);
        g.setDACResolution(dacResolution);
        usleep(1e6);
        g.finishR2();
    }
    usleep(defaultWait);

    qDebug() << "Start signal loop";
    for(SignalGenerator& g : generators())
        g.startSignalGeneration();
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
        if(!maskedDevices.contains(n) &&
                SignalManagerInt::i2cSendCommand(n, SystemRequest::Alive, 0))
        {
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
    generatorMap.clear();
    generatorList.clear();

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
            generatorList.append(SignalGenerator(n));
            generatorMap[n] = generatorList.count() - 1;
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

QVector<SignalGenerator>& SignalManager::generators()
{
    return generatorList;
}

QList<uint8_t> SignalManager::addresses() const
{
    return generatorMap.keys();
}

SignalGenerator& SignalManager::generator(uint8_t address)
{
    return generatorList[generatorMap[address]];
}
