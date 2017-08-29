#include "scratchpy.h"

#include <scratchy/signalmanager.h>

BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(reset_overloads, reset, 0, 1)

void export_signal_manager() {

    p::class_<SignalManager>("SignalManager")
            .def("initializeBoards", &SignalManager::initializeBoards)
            .def("scanDevices", &SignalManager::scanDevices)
            .def("reset", (void(*)(uint8_t))0, reset_overloads())
            .def("initSystem", &SignalManager::initSystem)
            .def("assignAddresses", &SignalManager::assignAddresses)
            .def("gatherSPISpeed", &SignalManager::gatherSPISpeed)
            .def("maskDevice", &SignalManager::maskDevice)
            .def("generators", &SignalManager::generators)
            .def("addresses", &SignalManager::addresses)
            .def("generator", &SignalManager::generator);
}
