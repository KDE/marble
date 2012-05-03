//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2012 Ralf Habacker <ralf.habacker@freenet.de>

#ifndef FLIGHTGEARPOSITIONPROVIDERPLUGIN_H
#define FLIGHTGEARPOSITIONPROVIDERPLUGIN_H

#include "PositionProviderPlugin.h"

#include <nmea/nmea.h>

class QUdpSocket;

namespace Marble
{

class FlightGearPositionProviderPlugin : public PositionProviderPlugin
{
    Q_OBJECT
    Q_INTERFACES( Marble::PositionProviderPluginInterface )

 public:
    FlightGearPositionProviderPlugin();
    virtual ~FlightGearPositionProviderPlugin();

    virtual QString name() const;
    virtual QString nameId() const;
    virtual QString guiString() const;
    virtual QString version() const;
    virtual QString description() const;
    virtual QString copyrightYears() const;
    virtual QList<PluginAuthor> pluginAuthors() const;
    virtual QIcon icon() const;
    virtual void initialize();
    virtual bool isInitialized() const;

    virtual PositionProviderPlugin * newInstance() const;

    virtual PositionProviderStatus status() const;
    virtual GeoDataCoordinates position() const;
    virtual GeoDataAccuracy accuracy() const;
    virtual QString error() const;
    virtual qreal speed() const;
    virtual qreal direction() const;
    virtual QDateTime timestamp() const;

 private:
    QUdpSocket* m_socket;
    nmeaPARSER m_parser;
    nmeaINFO m_info;
    PositionProviderStatus m_status;
    GeoDataCoordinates m_position;
    GeoDataAccuracy m_accuracy;
    qreal m_speed;
    qreal m_track;
    QDateTime m_timestamp;

 private slots:
    void readPendingDatagrams();
    void update();
};

}

#endif // FLIGHTGEARPOSITIONPROVIDERPLUGIN_H
