CONFIG     += qt debug
QT         += xml
TEMPALTE    = app

# Strict compilation
CONFIG -= warn_off
CONFIG -= warn_on
QMAKE_CFLAGS   += -Wall -W -Wno-unused -Wno-comment
QMAKE_CXXFLAGS += -Wall -W -Wno-unused -Wno-comment

DESTDIR     = ./Builds
OBJECTS_DIR = ./Builds/Temp

HEADERS = \
          data/GeoDataDocument.h \
          data/GeoDataFolder.h \
          parser/GeoDataParser.h \
          parser/GeoDataTagHandler.h \
          handlers/gpx/GPXgpxTagHandler.h \
          handlers/gpx/GPXElementDictionary.h \
          handlers/kml/KMLDocumentTagHandler.h \
          handlers/kml/KMLFolderTagHandler.h \
          handlers/kml/KMLElementDictionary.h
 
SOURCES = \ 
          data/GeoDataDocument.cpp \
          data/GeoDataFolder.cpp \
          parser/GeoDataParser.cpp \
          parser/GeoDataTagHandler.cpp \
          handlers/gpx/GPXgpxTagHandler.cpp \
          handlers/gpx/GPXElementDictionary.cpp \
          handlers/kml/KMLDocumentTagHandler.cpp \
          handlers/kml/KMLFolderTagHandler.cpp \
          handlers/kml/KMLElementDictionary.cpp \
          GeoDataTest.cpp
