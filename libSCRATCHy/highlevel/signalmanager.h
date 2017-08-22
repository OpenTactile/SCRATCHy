#ifndef SIGNALBOARDCTRL_H
#define SIGNALBOARDCTRL_H

#include <QVector>
#include <QMap>
#include <cstdint>

class GraphicalDisplay;
class SignalGenerator;

class SignalManager
{
public:
    SignalManager();
    ~SignalManager();

    bool initializeBoards(GraphicalDisplay* display = nullptr, unsigned int dacResolution = 9);

    // Get a list of used I2C addresses
    QVector<uint8_t> scanDevices();

    // Reset a specific SignalGenerator
    void reset(uint8_t address);

    // Reset all SignalGenerators
    void reset();

    // Initializes all idle SignalGenerators listening to address 0x0
    void initSystem();

    void assignAddresses();
    void gatherSPISpeed();

    void maskDevice(uint8_t address);

    QList<SignalGenerator *> generators();
    QList<uint8_t> addresses();

    SignalGenerator* generator(uint8_t address);

protected:
    QMap<uint8_t, SignalGenerator*> generatorMap;
    QVector<uint8_t> maskedDevices;
};

#endif // SIGNALBOARDCTRL_H
