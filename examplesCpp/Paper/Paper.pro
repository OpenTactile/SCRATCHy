QT += core

CONFIG += c++11

TARGET = Paper
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += main.cpp

INCLUDEPATH += /usr/include/scratchy
LIBS += -lSCRATCHy
