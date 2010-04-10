//
// This file is part of the Marble Desktop Globe.
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

    MaemoPositionProviderPluginPrivate();

    ~MaemoPositionProviderPluginPrivate();
};

MaemoPositionProviderPluginPrivate::MaemoPositionProviderPluginPrivate() :
        m_control( 0 ), m_device( 0 ), m_status( PositionProviderStatusUnavailable )
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

QString MaemoPositionProviderPlugin::description() const
{
    return tr( "Reports the GPS position of a Maemo device (e.g. Nokia N900)." );
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
    if ( status() == PositionProviderStatusAvailable ) {
        return GeoDataCoordinates( d->m_device->fix->longitude,
                                   d->m_device->fix->latitude,
                                   d->m_device->fix->altitude,
                                   GeoDataCoordinates::Degree );
    }

    return GeoDataCoordinates();
}

GeoDataAccuracy MaemoPositionProviderPlugin::accuracy() const
{
    GeoDataAccuracy result;

    // FIXME: I'm not sure what is expected here, the documentation in
    // Marble is a bit coarse and I did not find any class using it
    if ( status() == PositionProviderStatusAvailable ) {
        result.level = GeoDataAccuracy::Detailed;
        result.horizontal = d->m_device->fix->eph; // horizontal position accuracy in centimeter
        result.vertical = d->m_device->fix->epv; // vertical position accuracy in meter
    }
    else {
        result.level = GeoDataAccuracy::none;
        result.horizontal = 0;
        result.vertical = 0;
    }

    return result;
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
                 "preferred-method", LOCATION_METHOD_GNSS,
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
    PositionProviderStatus newStatus = PositionProviderStatusUnavailable;
    if ( d->m_device ) {
        newStatus = d->m_device->fix ? PositionProviderStatusAvailable : PositionProviderStatusAcquiring;
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
