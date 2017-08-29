#ifndef SCRATCHPY_H
#define SCRATCHPY_H

#include <boost/python.hpp>
namespace p = boost::python;

void initialize_converters();

void export_graphical_display();
void export_signal_generator();
void export_signal_manager();

#endif // SCRATCHPY_H
