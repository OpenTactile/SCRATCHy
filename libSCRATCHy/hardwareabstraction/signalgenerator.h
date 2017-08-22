#ifndef SIGNALGENERATOR_H
#define SIGNALGENERATOR_H

#include <cstdint>

const int FREQUENCY_COUNT_HW = 10;

using fixed_q5 = uint16_t;
using fixed_q15 = uint16_t;

struct FrequencyTable
{
    fixed_q5  frequency[FREQUENCY_COUNT_HW] = {0};
    fixed_q15 amplitude[FREQUENCY_COUNT_HW] = {0};
};

enum class SystemStatus
{
    Operational    = 0b0000,
    ThermalWarning = 0b0010,
    NotInSync      = 0b0100,
    ConfigInvalid  = 0b1000,
    NotResponding  = 0b1111
};

enum class SystemRequest
{
    // Can be received at any time
    Reset               = 0b11111111, // Performs a soft-reset using _reboot_Teensyduino_()

    // Can be received at any time
    Alive               = 0b00000000,
    Status              = 0b00000001,
    StatusReset         = 0b00000011,

    // Runlevel 0:
    // The Signalboard just has been booted, the internal I2C address
    // is set to 0b0000000 till the mainboard sets a new one.
    // A new address is only set if SCK is low after 'SetI2CAddress'
    // has been sent.
    FinishR0            = 0b00011111, // Advance to runlevel 1
    SetI2CAddress       = 0b00010000, // 2nd Byte contains the new address

    // Runlevel 1:
    // Performance measurements are taken out to determine the maximum
    // reliable speed for SPI communication. Several packages of 1kByte
    // size are sent by the mainboard via SPI and a checksum (CRC32) is
    // requested. This test can be performed several times.
    FinishR1            = 0b00101111, // Advance to runlevel 2
    SPIPrepare          = 0b00100001,
    SPIChecksum         = 0b00100000, // Calculate and send back CRC32 checksum

    // Runlevel 2:
    // Configuration of the signalgenerator
    FinishR2            = 0b00111111, // Advance to runlevel 3
    SetSamplingRate     = 0b00110000,
    SetDACResolution    = 0b00110001,
    SetPanicMask        = 0b00110010,

    // Runlevel 3:
    // Run the frequency generator
    FinishR3            = 0b01001111, // Advance to runlevel 4
    StartLoop           = 0b01000000,

    // Runlevel 4:
    // No operations are carried out at runlevel 4.
    // This mode is used after powering down the mainboard
    // Only 'Reset' and Status commands are accepted.

    // Runlevel R (5):
    // Recovery state: Whenever the SPI transfer is out of sync, the Teensy
    // switches to "recovery mode" allowing to resync with the mainboard
    FinishRecovery      = 0b01011111,

    // Runlevel P (6):
    // Panic: A fatal error has occured, no further operation is performed
    // till the module has been resetted

    // Runlevel -1:
    // Wait for wakeupcall from main board
    InitSystem          = 0b10101010
};


class SignalGenerator
{
public:
    SignalGenerator(uint8_t address);
    ~SignalGenerator();

    uint8_t address() const;

    SystemStatus status() const;
    void resetStatus() const;
    bool isAlive();

    // Runlevel 0 only
    void finishR0();

    // Runlevel 1 only
    bool spiCheck(uint16_t divider = 16) const;
    void finishR1();


    // Runlevel 2 only
    void setDACResolution(uint8_t resolution);
    void setSamplingRate(uint32_t rate);
    void setPanicMask(uint8_t mask);
    void finishR2();

    // Runlevel 3 only
    void startSignalGeneration() const;
    void sendTables(const FrequencyTable *data);
    void shutdown(); // Advance to Runlevel 4

private:
    uint8_t addr;
    unsigned char runlevel;
};

#endif // SIGNALGENERATOR_H
