#-------------------------------------------------
#
# Project created by QtCreator 2016-01-25T09:31:12
#
#-------------------------------------------------

QT       = gui core
TARGET   = SCRATCHy
TEMPLATE = lib
CONFIG  += c++11

# Use this option to build libSCRATCHy with a non-functional IOWrapper,
# thus allowing to use it on a standard pc.
#CONFIG += fake

INSTALL_PATH_LIB = /usr/lib
INSTALL_PATH_INCLUDE = /usr/include/scratchy

DEFINES += LIBSCRATCHY_LIBRARY

SOURCES += \
    external/rastafont/rastafont8x8.c \
    external/rastafont/rastafont8x12.c \
    external/rastafont/rastafont6x8.c \
    lowlevel/oleddisplay.cpp \
    lowlevel/mouseevents.cpp \
    lowlevel/imagehandlingoled.cpp \
    positionproviders/mouseeventtransmitter.cpp \
    positionproviders/mouseestimator.cpp \
    hardwareabstraction/graphicaldisplay.cpp \
    hardwareabstraction/signalgenerator.cpp \
    hardwareabstraction/signalmanager.cpp \
    positionproviders/constantvelocityquery.cpp \
    positionproviders/mousepositionquery.cpp     

fake {
    SOURCES += lowlevel/iowrap_dummy.cpp
    QMAKE_CXXFLAGS_RELEASE += -Og -g
}

!fake {
    SOURCES += lowlevel/iowrap_raspberry.cpp
    QMAKE_CXXFLAGS_RELEASE += -O3
}

HEADERS +=\
    external/rastafont/rastafont.h \
    positionquery.h \
    lowlevel/oleddisplay.h \
    lowlevel/mouseevents.h \
    lowlevel/imagehandlingoled.h \
    positionproviders/mouseeventtransmitter.h \
    positionproviders/mouseestimator.h \
    hardwareabstraction/graphicaldisplay.h \
    hardwareabstraction/signalgenerator.h \
    hardwareabstraction/signalmanager.h \
    positionproviders/constantvelocityquery.h \
    positionproviders/mousepositionquery.h \    
    lowlevel/iowrap.h \
    scratchy.h


LIBS += -lusb -latomic

!fake {
    LIBS += -lwiringPi -lbcm2835
}

RESOURCES += \
    ressources.qrc

unix {
    target.path = $${INSTALL_PATH_LIB}
    header_files.path = $${INSTALL_PATH_INCLUDE}
    header_files.files = scratchy.h\
        hardwareabstraction/signalgenerator.h \
        hardwareabstraction/signalmanager.h \
        hardwareabstraction/graphicaldisplay.h \
        positionquery.h \
        positionproviders/constantvelocityquery.h \
        positionproviders/mousepositionquery.h \
        positionproviders/tactilemousepositionquery.h \
        lowlevel/iowrap.h

    INSTALLS += target header_files
}
