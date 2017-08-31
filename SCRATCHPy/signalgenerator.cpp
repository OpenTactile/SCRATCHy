#include "scratchpy.h"

#include <scratchy/signalgenerator.h>

std::string reprSignalGenerator(SignalGenerator const& gen) {
    return std::string("<SignalGenerator(Address: ") + std::to_string(gen.address()) + ")>";
}

void export_signal_generator() {
    p::enum_<SystemStatus>("SystemStatus")
            .value("Operational", SystemStatus::Operational)
            .value("ThermalWarning", SystemStatus::ThermalWarning)
            .value("NotInSync", SystemStatus::NotInSync)
            .value("ConfigInvalid", SystemStatus::ConfigInvalid)
            .value("NotResponding", SystemStatus::NotResponding);

    void (SignalGenerator::*send1)(const std::array<FrequencyTable, 4>&) = &SignalGenerator::send;
    void (SignalGenerator::*send2)(const FrequencyTable&) = &SignalGenerator::send;
    void (SignalGenerator::*send3)(const FrequencyTable&, const FrequencyTable&,
              const FrequencyTable&, const FrequencyTable&) = &SignalGenerator::send;

    p::class_<SignalGenerator>("SignalGenerator", p::init<uint8_t>())
            .def("address", &SignalGenerator::address)
            .def("status", &SignalGenerator::status)
            .def("resetStatus", &SignalGenerator::resetStatus)
            .def("isAlive", &SignalGenerator::isAlive)
            .def("finishR0", &SignalGenerator::finishR0)
            .def("spiCheck", &SignalGenerator::spiCheck)
            .def("finishR1", &SignalGenerator::finishR1)
            .def("setDACResolution", &SignalGenerator::setDACResolution)
            .def("setSamplingRate", &SignalGenerator::setSamplingRate)
            .def("finishR2", &SignalGenerator::finishR2)
            .def("startSignalGeneration", &SignalGenerator::startSignalGeneration)
            .def("send", send1)
            .def("send", send2)
            .def("send", send3)
            .def("shutdown", &SignalGenerator::shutdown)
            .def("__repr__", reprSignalGenerator);
}
