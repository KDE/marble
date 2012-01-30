#-------------------------------------------------
#
# Project created by QtCreator 2012-01-08T00:53:03
#
#-------------------------------------------------

QT       += core

QT       -= gui

TARGET = mapreproject
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp \
    NasaWorldWindToOpenStreetMapConverter.cpp \
    NwwMapImage.cpp \
    OsmTileClusterRenderer.cpp \
    Thread.cpp \
    ReadOnlyMapImage.cpp \
    ReadOnlyMapDefinition.cpp \
    SimpleMapImage.cpp \
    InterpolationMethod.cpp \
    BilinearInterpolation.cpp \
    NearestNeighborInterpolation.cpp

HEADERS += \
    NasaWorldWindToOpenStreetMapConverter.h \
    NwwMapImage.h \
    OsmTileClusterRenderer.h \
    Thread.h \
    mapreproject.h \
    ReadOnlyMapImage.h \
    ReadOnlyMapDefinition.h \
    SimpleMapImage.h \
    InterpolationMethod.h \
    BilinearInterpolation.h \
    NearestNeighborInterpolation.h

unix|win32: LIBS += -lQtGui

QMAKE_CXXFLAGS += -march=native
