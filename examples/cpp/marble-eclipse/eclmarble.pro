TEMPLATE = app
TARGET = eclmarble
DEPENDPATH += .
INCLUDEPATH += .

HEADERS += astrolib.h \
           attlib.h \
           eclsolar.h
SOURCES += eclmarble.cpp \
           astrolib.cpp \
           attlib.cpp \
           eclsolar.cpp
LIBS += -lmarblewidget 
