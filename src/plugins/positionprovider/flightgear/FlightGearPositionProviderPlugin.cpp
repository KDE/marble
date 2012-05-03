//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2012 Ralf Habacker <ralf.habacker@freenet.de>
//

#include "FlightGearPositionProviderPlugin.h"

#include "MarbleDebug.h"
#include <math.h>

#include <QUdpSocket>

using namespace Marble;
/* TRANSLATOR Marble::FlightGearPositionProviderPlugin */

using namespace std;

FlightGearPositionProviderPlugin::FlightGearPositionProviderPlugin()
  : m_socket(0), m_speed( 0.0 ), m_track( 0.0 )
{
}

FlightGearPositionProviderPlugin::~FlightGearPositionProviderPlugin()
{
    delete m_socket;
    nmea_parser_destroy(&m_parser);
}

QString FlightGearPositionProviderPlugin::name() const
{
    return tr( "FlightGear position provider Plugin" );
}

QString FlightGearPositionProviderPlugin::nameId() const
{
    return QString::fromLatin1( "flightgear" );
}

QString FlightGearPositionProviderPlugin::guiString() const
{
    return tr( "FlightGear" );
}

QString FlightGearPositionProviderPlugin::version() const
{
    return "1.0";
}

QString FlightGearPositionProviderPlugin::description() const
{
    return tr( "Reports the position of running flightgear application." );
}

QString FlightGearPositionProviderPlugin::copyrightYears() const
{
    return "2012";
}

QList<PluginAuthor> FlightGearPositionProviderPlugin::pluginAuthors() const
{
    return QList<PluginAuthor>()
            << PluginAuthor( QString::fromUtf8( "Ralf Habacker" ), "ralf.habacker@freenet.de" );

}

QIcon FlightGearPositionProviderPlugin::icon() const
{
    return QIcon();
}

void FlightGearPositionProviderPlugin::initialize()
{
    m_status = PositionProviderStatusAcquiring;
    emit statusChanged( m_status );

    m_socket = new QUdpSocket(this);
    m_socket->bind(QHostAddress::LocalHost, 5500);
    nmea_parser_init(&m_parser);
    nmea_zero_INFO(&m_info);

    connect(m_socket, SIGNAL(readyRead()),
             this, SLOT(readPendingDatagrams()));
}

/**
 fixed case where wrong date format is used '2404112' instead of '240412'
*/
bool fixBadGPRMC(QByteArray &line)
{
    if (!line.startsWith("$GPRMC"))
        return false;

    QStringList parts = QString(line).split(',');
    if (parts[9].size() == 7) {
        parts[9].remove(4,1);
        line = parts.join(",").toLatin1();
        // update crc
        int crc = 0;
        for(int i=1; i < line.size()-3; i++) {
            crc ^= (int) line[i];
        }
        parts[11] = parts[11][0] + parts[11][1] +  QString::number(crc, 16).toUpper();

        line = parts.join(",").toLatin1();
        return true;
    }
    return false;
}

void FlightGearPositionProviderPlugin::readPendingDatagrams()
{
    while (m_socket->hasPendingDatagrams()) {
        QByteArray datagram;
        datagram.resize(m_socket->pendingDatagramSize());
        QHostAddress sender;
        quint16 senderPort;

        m_socket->readDatagram(datagram.data(), datagram.size(), &sender, &senderPort);
        foreach(QByteArray line, datagram.split('\n')) {
            fixBadGPRMC(line);
            //qDebug() << line;
            line.append("\r\n");
            nmea_parse(&m_parser, line.data(), line.size(), &m_info);
            update();
        }
    }
}

void FlightGearPositionProviderPlugin::update()
{
    PositionProviderStatus oldStatus = m_status;
    GeoDataCoordinates oldPosition = m_position;
    if ( m_info.sig == 0 )
        m_status = PositionProviderStatusUnavailable;
    else {
        m_status = PositionProviderStatusAvailable;

        // fg atlas nmea output uses feet unit, which is not covered by nmealib <= 0.5.3
#ifndef NMEA_TUD_FEED
#define NMEA_TUD_FEED       (1/0.3048)      /**< Feet, meter / NMEA_TUD_FEED = feet */
        qreal elevation = m_info.elv / NMEA_TUD_FEED;
#else
        qreal elevation = m_info.elv;
#endif
        m_position.set( nmea_ndeg2degree(m_info.lon), nmea_ndeg2degree(m_info.lat), elevation, GeoDataCoordinates::Degree );
        m_accuracy.level = GeoDataAccuracy::Detailed;
        // FIX  for misinterpreting
        m_speed = m_info.speed * 0.51444444 / NMEA_TUD_KNOTS ;
        m_track = m_info.direction;
    }
    if (m_status != oldStatus)
        emit statusChanged( m_status );
    if (!(oldPosition == m_position)) {
        emit positionChanged( m_position, m_accuracy );
    }
}

bool FlightGearPositionProviderPlugin::isInitialized() const
{
    return m_socket;
}

PositionProviderPlugin* FlightGearPositionProviderPlugin::newInstance() const
{
    return new FlightGearPositionProviderPlugin;
}

PositionProviderStatus FlightGearPositionProviderPlugin::status() const
{
    return m_status;
}

GeoDataCoordinates FlightGearPositionProviderPlugin::position() const
{
    return m_position;
}

GeoDataAccuracy FlightGearPositionProviderPlugin::accuracy() const
{
    return m_accuracy;
}

qreal FlightGearPositionProviderPlugin::speed() const
{
    return m_speed;
}

qreal FlightGearPositionProviderPlugin::direction() const
{
    return m_track;
}

QDateTime FlightGearPositionProviderPlugin::timestamp() const
{
    return m_timestamp;
}


QString FlightGearPositionProviderPlugin::error() const
{
    return QString();
}


Q_EXPORT_PLUGIN2( FlightGearPositionProviderPlugin, Marble::FlightGearPositionProviderPlugin )



#include "FlightGearPositionProviderPlugin.moc"
