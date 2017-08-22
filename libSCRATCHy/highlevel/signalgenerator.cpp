#include "signalgenerator.h"

#include "lowlevel/iowrap.h"

#include <QDebug>
#include <random>
#include <boost/crc.hpp>


namespace SignalGeneratorInt
{
    bool i2cSendCommand(unsigned char targetAddress, SystemRequest command, unsigned int payload)
    {
        const unsigned char* orig = reinterpret_cast<const unsigned char*>(&payload);
        const unsigned char conv[4] =
        {
            orig[3], orig[2], orig[1], orig[0]
        };

        return IO::I2CSetAddress(targetAddress) &&
               IO::I2CWriteBlock(static_cast<int>(command), 4, conv);
    }

    bool i2cQueryCommand(unsigned char targetAddress, SystemRequest command, unsigned int& payload)
    {
        return IO::I2CSetAddress(targetAddress) &&
               IO::I2CReadBlock(static_cast<int>(command), sizeof(unsigned int), reinterpret_cast<unsigned char*>(&payload));
    }

    bool i2cQueryCommandLong(unsigned char targetAddress, SystemRequest command, unsigned long& payload)
    {
        return IO::I2CSetAddress(targetAddress) &&
               IO::I2CReadBlock(static_cast<int>(command), sizeof(unsigned long), reinterpret_cast<unsigned char*>(&payload));
    }

    unsigned long crc32(const unsigned char* data, unsigned int length)
    {
        boost::crc_32_type result;
        result.process_bytes(data, length);
        return result.checksum();
    }
}

SignalGenerator::SignalGenerator(uint8_t address)
{
    this->addr = address;
    //timer.start(1000, Qt::CoarseTimer, this);
}

SignalGenerator::~SignalGenerator()
{

}

uint8_t SignalGenerator::address() const
{
    return addr;
}

SystemStatus SignalGenerator::status() const
{
    unsigned int stat = 0;

    bool respond = SignalGeneratorInt::i2cQueryCommand(addr, SystemRequest::Status, stat);

    if(!respond)
        return SystemStatus::NotResponding;

    switch (static_cast<SystemStatus>(stat)) {
    case SystemStatus::Operational:
        return SystemStatus::Operational;

    case SystemStatus::NotInSync:
        return SystemStatus::NotInSync;

    case SystemStatus::ConfigInvalid:
        return SystemStatus::ConfigInvalid;

    case SystemStatus::ThermalWarning:
        return SystemStatus::ThermalWarning;

    default:
        return SystemStatus::NotResponding;
    }
}

void SignalGenerator::resetStatus() const
{
    SignalGeneratorInt::i2cSendCommand(addr, SystemRequest::StatusReset, 0);
}

bool SignalGenerator::isAlive()
{
    return SignalGeneratorInt::i2cSendCommand(addr, SystemRequest::Alive, 0);
}


bool SignalGenerator::spiCheck(unsigned short divider) const
{
    sleep(5);
    SignalGeneratorInt::i2cSendCommand(addr, SystemRequest::SPIPrepare, 0);
    // Set SPI speed
    IO::SPISetDivider(divider);
    qDebug() << "Testing SPI speed of" << IO::SPIGetCurrentSpeed()/1000.0 << "MHz";

    // Generate random test data
    unsigned char data[128];
    for(int n = 0; n < 128; n++)
    {
        data[n] = rand();
    }

    unsigned long crcGoal = SignalGeneratorInt::crc32(data, 128);

    sleep(5);
    IO::GPIOSetAddress(addr);
    IO::SPISend(reinterpret_cast<const uint16_t*>(data), sizeof(unsigned char) * 128);
    IO::GPIOSetAddress(0);

    sleep(5);

    unsigned long crcTeensy = -1;
    bool success = SignalGeneratorInt::i2cQueryCommandLong(addr, SystemRequest::SPIChecksum, crcTeensy);
    exit(0);
    if(!success)
    {
        qDebug() << "ERROR: Signalboard No." << addr << " did not respond to crc check!";
        return false;
    }
    else
    {
        if(crcTeensy == crcGoal)
        {
            qDebug() << "MATCH - Got CRC response:" << crcTeensy;
            return true;
        }
        else
        {
            qDebug() << "FAILED - Got CRC response:" << crcTeensy << "should be" << crcGoal;
            return false;
        }
    }
    return false;
}

void SignalGenerator::finishR0()
{
    SignalGeneratorInt::i2cSendCommand(addr, SystemRequest::FinishR0, 0);
}

void SignalGenerator::finishR1()
{
    SignalGeneratorInt::i2cSendCommand(addr, SystemRequest::FinishR1, 0);
}


void SignalGenerator::setDACResolution(unsigned char resolution)
{
    SignalGeneratorInt::i2cSendCommand(addr, SystemRequest::SetDACResolution, resolution);
}

void SignalGenerator::setSamplingRate(unsigned int rate)
{
    SignalGeneratorInt::i2cSendCommand(addr, SystemRequest::SetSamplingRate, rate);
}

void SignalGenerator::setPanicMask(unsigned char mask)
{
    Q_UNUSED(mask);
    // Not implemented, yet
}

void SignalGenerator::finishR2()
{
    SignalGeneratorInt::i2cSendCommand(addr, SystemRequest::FinishR2, 0);
}


void SignalGenerator::startSignalGeneration() const
{
    SignalGeneratorInt::i2cSendCommand(addr, SystemRequest::StartLoop, 0);
}

void SignalGenerator::sendTables(const FrequencyTable* data)
{
    IO::GPIOSetAddress(addr);
    usleep(65);
    IO::SPISend(reinterpret_cast<const uint16_t*>(data), sizeof(FrequencyTable) * 4);
    IO::GPIOSetAddress(0);
    usleep(65);
}

void SignalGenerator::shutdown()
{
    SignalGeneratorInt::i2cSendCommand(addr, SystemRequest::FinishR3, 0);
}
