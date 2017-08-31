#include "scratchpy.h"

#include "containerwrapper.h"

template<typename T>
p::str showValue(T const& x) {
    p::str res = "<";
    res += p::object(x).attr("__class__").attr("__name__");
    res += + "(" + p::str(float(x)) +")>";
    return res;
}

void export_datatypes() {
    p::class_<fixed_q5>("fixed_q5", p::init<double>())
            .add_property("value", &fixed_q5::operator float, &fixed_q5::setValue<double>)
            .def("__repr__", showValue<fixed_q5>);

    p::class_<fixed_q15>("fixed_q15", p::init<double>())
            .add_property("value", &fixed_q15::operator float, &fixed_q15::setValue<double>)
            .def("__repr__", showValue<fixed_q15>);

    p::class_<FrequencyTable>("FrequencyTable")
            .def_readwrite("frequency", &FrequencyTable::frequency)
            .def_readwrite("amplitude", &FrequencyTable::amplitude);

    wrap_idx_container<std::array<fixed_q5, 10>>("Array10Q5");
    wrap_idx_container<std::array<fixed_q15, 10>>("Array10Q15");
    wrap_idx_container<std::array<FrequencyTable, 4>>("FrequencyTableArray");

    wrap_list_container<std::vector<SignalGenerator>>("SignalGeneratorVector");
//    wrap_list_container<std::vector<uint8_t>>("AddressVector");
}
