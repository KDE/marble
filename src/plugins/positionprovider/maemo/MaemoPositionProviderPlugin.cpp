//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010        Dennis Nienhüser <earthwings@gentoo.org>
//

#include "MaemoPositionProviderPlugin.h"

#include <QtCore/QTimer>

#include <location/location-gps-device.h>
#include <location/location-gpsd-control.h>

namespace Marble {

class MaemoPositionProviderPluginPrivate
{
public:
    LocationGPSDControl *m_control;
    LocationGPSDevice *m_device;
    PositionProviderStatus m_status;
    QTimer m_timer;
    qreal m_speed;
    qreal m_direction;
    QDateTime m_timestamp;

    MaemoPositionProviderPluginPrivate();

    ~MaemoPositionProviderPluginPrivate();
};

MaemoPositionProviderPluginPrivate::MaemoPositionProviderPluginPrivate() :
        m_control( 0 ), m_device( 0 ), m_status( PositionProviderStatusAcquiring ),
    m_speed( 0 ), m_direction( 0 )
{
    m_timer.setInterval( 1000 );
}

MaemoPositionProviderPluginPrivate::~MaemoPositionProviderPluginPrivate()
{
    g_object_unref( m_device );
    g_object_unref( m_control );
}

QString MaemoPositionProviderPlugin::name() const
{
    return tr( "Maemo Position Provider Plugin" );
}

QString MaemoPositionProviderPlugin::nameId() const
{
    return "MaemoPositionProvider";
}

QString MaemoPositionProviderPlugin::guiString() const
{
    return tr( "Maemo" );
}

QString MaemoPositionProviderPlugin::version() const
{
    return "1.0";
}

QString MaemoPositionProviderPlugin::description() const
{
    return tr( "Reports the GPS position of a Maemo device (e.g. Nokia N900)." );
}

QString MaemoPositionProviderPlugin::copyrightYears() const
{
    return "2010";
}

QList<PluginAuthor> MaemoPositionProviderPlugin::pluginAuthors() const
{
    return QList<PluginAuthor>()
            << PluginAuthor( QString::fromUtf8( "Dennis Nienhüser" ), "earthwings@gentoo.org" );
}

QIcon MaemoPositionProviderPlugin::icon() const
{
    return QIcon();
}

PositionProviderPlugin* MaemoPositionProviderPlugin::newInstance() const
{
    return new MaemoPositionProviderPlugin;
}

PositionProviderStatus MaemoPositionProviderPlugin::status() const
{
    return d->m_status;
}

GeoDataCoordinates MaemoPositionProviderPlugin::position() const
{
    if ( status() == PositionProviderStatusAvailable &&
         d->m_device->fix->fields & LOCATION_GPS_DEVICE_LATLONG_SET ) {
        qreal alt = 0.0;
        if ( d->m_device->fix->fields & LOCATION_GPS_DEVICE_ALTITUDE_SET ) {
            alt = d->m_device->fix->altitude;
        }
        if ( d->m_device->fix->fields & LOCATION_GPS_DEVICE_SPEED_SET ) {
            d->m_speed = d->m_device->fix->speed;
        }
        if ( d->m_device->fix->fields & LOCATION_GPS_DEVICE_TRACK_SET ) {
            d->m_direction = d->m_device->fix->track;
        }
        if ( d->m_device->fix->fields & LOCATION_GPS_DEVICE_TIME_SET ) {
            d->m_timestamp = QDateTime::fromMSecsSinceEpoch( d->m_device->fix->time * 1000 );
        }

        return GeoDataCoordinates( d->m_device->fix->longitude,
                                   d->m_device->fix->latitude,
                                   alt, GeoDataCoordinates::Degree );
    }

    return GeoDataCoordinates();
}

qreal MaemoPositionProviderPlugin::speed() const
{
    return d->m_speed * KM2METER / HOUR2SEC;
}

qreal MaemoPositionProviderPlugin::direction() const
{
    return d->m_direction;
}

GeoDataAccuracy MaemoPositionProviderPlugin::accuracy() const
{
    GeoDataAccuracy result;

    if ( status() == PositionProviderStatusAvailable ) {
        result.level = GeoDataAccuracy::Detailed;
        result.horizontal = d->m_device->fix->eph / 100.0; // cm => meter
        result.vertical = d->m_device->fix->epv; // meter
    }
    else {
        result.level = GeoDataAccuracy::none;
        result.horizontal = 0;
        result.vertical = 0;
    }

    return result;
}

QDateTime MaemoPositionProviderPlugin::timestamp() const
{
    return d->m_timestamp;
}

MaemoPositionProviderPlugin::MaemoPositionProviderPlugin() :
        d( new MaemoPositionProviderPluginPrivate )
{
    connect( &d->m_timer, SIGNAL( timeout() ), this, SLOT( update() ) );
}

MaemoPositionProviderPlugin::~MaemoPositionProviderPlugin()
{
    delete d;
}

void MaemoPositionProviderPlugin::initialize()
{
    // See http://wiki.maemo.org/Documentation/Maemo_5_Developer_Guide/Using_Connectivity_Components/Using_Location_API
    d->m_control = location_gpsd_control_get_default();
    d->m_device = reinterpret_cast<LocationGPSDevice*>( g_object_new( LOCATION_TYPE_GPS_DEVICE, NULL ) );
    g_object_set( G_OBJECT( d->m_control ),
                 "preferred-method", LOCATION_METHOD_USER_SELECTED,
                 "preferred-interval", LOCATION_INTERVAL_1S,
                 NULL);
    location_gpsd_control_start( d->m_control );

    // Poll and report the GPS position each second.
    d->m_timer.start();
}

bool MaemoPositionProviderPlugin::isInitialized() const
{
    return d->m_control != 0;
}

void MaemoPositionProviderPlugin::update()
{
    PositionProviderStatus newStatus = PositionProviderStatusAcquiring;
    if ( d->m_device ) {
        if ( d->m_device->status == LOCATION_GPS_DEVICE_STATUS_FIX && d->m_device->fix )
            newStatus = PositionProviderStatusAvailable;
        else
            newStatus = PositionProviderStatusUnavailable;
    }

    if ( newStatus != d->m_status ) {
        d->m_status = newStatus;
        emit statusChanged( newStatus );
    }

    if ( newStatus == PositionProviderStatusAvailable ) {
        emit positionChanged( position(), accuracy() );
    }
}

} // namespace Marble

Q_EXPORT_PLUGIN2( Marble::MaemoPositionProviderPlugin, Marble::MaemoPositionProviderPlugin )

#include "MaemoPositionProviderPlugin.moc"
