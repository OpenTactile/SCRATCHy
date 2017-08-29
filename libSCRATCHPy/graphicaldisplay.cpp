#include "scratchpy.h"

#include <scratchy/graphicaldisplay.h>

void export_graphical_display() {

    void (GraphicalDisplay::*show1)(Icon, const std::string&, const std::string&) = &GraphicalDisplay::show;
    void (GraphicalDisplay::*show2)(Icon, const std::string&, float) = &GraphicalDisplay::show;

    p::scope in_GraphicalDisplay =  p::class_<GraphicalDisplay>("GraphicalDisplay")
                                        .def("isPressed",&GraphicalDisplay::isPressed)
                                        .def("clear",&GraphicalDisplay::clear)
                                        .def("show",show1)
                                        .def("show",show2)
                                        .def("text", &GraphicalDisplay::text);

    p::enum_<Icon>("Icon")
            .value("None",Icon::None)
            .value("wlLogo",Icon::wlLogo)
            .value("Logo",Icon::Logo)
            .value("Spectrum",Icon::Spectrum)
            .value("Forbidden",Icon::Forbidden)
            .value("Bolt",Icon::Bolt)
            .value("Bug",Icon::Bug)
            .value("Check",Icon::Check)
            .value("Clock",Icon::Clock)
            .value("Cog",Icon::Cog)
            .value("Fire",Icon::Fire)
            .value("Image",Icon::Image)
            .value("Reboot",Icon::Reboot)
            .value("Power",Icon::Power)
            .value("Pulse",Icon::Pulse)
            .value("Random",Icon::Random)
            .value("Warning",Icon::Warning)
            .value("Wrench",Icon::Wrench)
            .value("X",Icon::X);

    p::enum_<Button>("Button")
            .value("Back",Button::Back)
            .value("Up",Button::Up)
            .value("Down",Button::Down)
            .value("Select",Button::Select);
}
