#-------------------------------------------------
#
# Project created by QtCreator 2012-01-08T00:53:03
#
#-------------------------------------------------

QT       += core

QT       -= gui

TARGET = NasaWorldWind2Osm
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp \
    NasaWorldWindToOpenStreetMapConverter.cpp

HEADERS += \
    NasaWorldWindToOpenStreetMapConverter.h

unix|win32: LIBS += -lQtGui

QMAKE_CXXFLAGS += -march=native
