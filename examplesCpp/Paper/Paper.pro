QT += core

CONFIG += c++11

TARGET = Paper
CONFIG += console
CONFIG -= app_bundle

QMAKE_CXXFLAGS_WARN_ON -= -Wall
QMAKE_CXXFLAGS += -Wno-unused-variable

TEMPLATE = app

SOURCES += main.cpp

INCLUDEPATH += /usr/include/scratchy
INCLUDEPATH += /usr/include/itchy
LIBS += -lSCRATCHy -lITCHy


