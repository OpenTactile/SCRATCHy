#ifndef SCRATCHPY_H
#define SCRATCHPY_H

#include <boost/python.hpp>
namespace p = boost::python;

#include <scratchy/graphicaldisplay.h>
#include <scratchy/signalgenerator.h>
#include <scratchy/signalmanager.h>

void initialize_converters();

void export_graphical_display();
void export_signal_generator();
void export_signal_manager();
void export_datatypes();

#endif // SCRATCHPY_H
