#-------------------------------------------------
#
# Project created by QtCreator 2011-12-13T19:20:21
#
#-------------------------------------------------

QT       += core xml sql

QT       -= gui

TARGET = osm-sisyphus
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp \
    jobqueue.cpp \
    job.cpp \
    jobmanager.cpp \
    jobparameters.cpp \
    region.cpp \
    logger.cpp \
    upload.cpp

HEADERS += \
    jobqueue.h \
    job.h \
    jobmanager.h \
    jobparameters.h \
    region.h \
    logger.h \
    upload.h












