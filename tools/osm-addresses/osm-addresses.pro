QT       += core xml sql gui

TARGET = osm-addresses
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

# Adjust these according to your system

# Marble include dir
INCLUDEPATH += /home/dennis/marble/export-git/include

# Marble include dir of the local osm search plugin
INCLUDEPATH += /home/dennis/marble/src-git/src/plugins/runner/local-osm-search

# Additional marble includes (not exported)
INCLUDEPATH += /home/dennis/marble/src-git/src/lib
INCLUDEPATH += /home/dennis/marble/src-git/src/lib/geodata
INCLUDEPATH += /home/dennis/marble/src-git/src/lib/geodata/parser
INCLUDEPATH += /home/dennis/marble/src-git/src/lib/geodata/data

# Marble lib path and library
LIBS += -L/home/dennis/marble/export-git/lib -lmarblewidget

# Marble local osm search plugin lib path and library
LIBS += -L/home/dennis/marble/export-git/lib/marble/plugins -lLocalOsmSearchPlugin

# Google's protobuf library
LIBS += -lprotobuf

SOURCES += main.cpp \
    OsmParser.cpp \
    Writer.cpp \
    SqlWriter.cpp \
    OsmRegion.cpp \
    OsmRegionTree.cpp \
    pbf/fileformat.pb.cc \
    pbf/osmformat.pb.cc \
    pbf/PbfParser.cpp \
    xml/XmlParser.cpp

HEADERS += \
    OsmParser.h \
    Writer.h \
    SqlWriter.h \
    OsmRegion.h \
    OsmRegionTree.h \
    pbf/osmformat.pb.h \
    pbf/fileformat.pb.h \
    pbf/PbfParser.h \
    xml/XmlParser.h
