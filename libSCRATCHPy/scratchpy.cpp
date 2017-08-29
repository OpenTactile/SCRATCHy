#include "scratchpy.h"

BOOST_PYTHON_MODULE(SCRATCHPy)
{
    initialize_converters();
    export_graphical_display();
    export_signal_generator();
    export_signal_manager();
}
