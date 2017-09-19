#include "scratchpy.h"

BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(SignalManager_initializeBoards_overloads, SignalManager::initializeBoards, 0, 2)

void export_signal_manager() {

    void (SignalManager::*reset1)() = &SignalManager::reset;
    void (SignalManager::*reset2)(uint8_t) = &SignalManager::reset;

    p::class_<SignalManager>("SignalManager")
            .def("initializeBoards", &SignalManager::initializeBoards,
                 SignalManager_initializeBoards_overloads())
            .def("scanDevices", &SignalManager::scanDevices)
            .def("reset",reset1)
            .def("reset",reset2)
            .def("initSystem", &SignalManager::initSystem)
            .def("assignAddresses", &SignalManager::assignAddresses)
            .def("gatherSPISpeed", &SignalManager::gatherSPISpeed)
            .def("maskDevice", &SignalManager::maskDevice)
            .def("generators", &SignalManager::generators, p::return_value_policy<p::reference_existing_object>())
            .def("addresses", &SignalManager::addresses)
            .def("generator", &SignalManager::generator, p::return_value_policy<p::reference_existing_object>());
}
