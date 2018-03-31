//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009      Eckhart Wörner <ewoerner@kde.org>
//

#include "GpsdPositionProviderPlugin.h"

#include "GpsdThread.h"
#include "MarbleDebug.h"

#include <QIcon>

#include <cmath>

using namespace Marble;
/* TRANSLATOR Marble::GpsdPositionProviderPlugin */

QString GpsdPositionProviderPlugin::name() const
{
    return tr( "Gpsd position provider Plugin" );
}

QString GpsdPositionProviderPlugin::nameId() const
{
    return QStringLiteral("Gpsd");
}

QString GpsdPositionProviderPlugin::guiString() const
{
    return tr( "gpsd" );
}

QString GpsdPositionProviderPlugin::version() const
{
    return QStringLiteral("1.0");
}

QString GpsdPositionProviderPlugin::description() const
{
    return tr( "Reports the position of a GPS device." );
}

QString GpsdPositionProviderPlugin::copyrightYears() const
{
    return QStringLiteral("2009");
}

QVector<PluginAuthor> GpsdPositionProviderPlugin::pluginAuthors() const
{
    return QVector<PluginAuthor>()
            << PluginAuthor(QStringLiteral("Eckhart Wörner"), QStringLiteral("ewoerner@kde.org"));

}

QIcon GpsdPositionProviderPlugin::icon() const
{
    return QIcon();
}

void GpsdPositionProviderPlugin::initialize()
{
    m_status = PositionProviderStatusAcquiring;
    emit statusChanged( m_status );

    m_thread = new GpsdThread;
    connect( m_thread, SIGNAL(gpsdInfo(gps_data_t)),
             this, SLOT(update(gps_data_t)) );
    connect( m_thread, SIGNAL(statusChanged(PositionProviderStatus)),
             this, SIGNAL(statusChanged(PositionProviderStatus)) );
    m_thread->start();
}

void GpsdPositionProviderPlugin::update( gps_data_t data )
{
    PositionProviderStatus oldStatus = m_status;
    GeoDataCoordinates oldPosition = m_position;
    if ( data.status == STATUS_NO_FIX || std::isnan( data.fix.longitude ) || std::isnan( data.fix.latitude ) )
        m_status = PositionProviderStatusAcquiring;
    else {
        m_status = PositionProviderStatusAvailable;
        m_position.set( data.fix.longitude, data.fix.latitude,
                        data.fix.altitude, GeoDataCoordinates::Degree );
        if (data.fix.mode == MODE_2D) {
            m_position.setAltitude(0);
        }

        m_accuracy.level = GeoDataAccuracy::Detailed;
#if defined( GPSD_API_MAJOR_VERSION ) && ( GPSD_API_MAJOR_VERSION >= 3 )
        if ( !std::isnan( data.fix.epx ) && !std::isnan( data.fix.epy ) ) {
            m_accuracy.horizontal = qMax( data.fix.epx, data.fix.epy );
        }
#else
        if ( !std::isnan( data.fix.eph ) ) {
            m_accuracy.horizontal = data.fix.eph;
        }
#endif
        if ( !std::isnan( data.fix.epv ) ) {
            m_accuracy.vertical = data.fix.epv;
        }

        if( !std::isnan(data.fix.speed ) )
        {
            m_speed = data.fix.speed;
        }

        if( !std::isnan( data.fix.track ) )
        {
            m_track = data.fix.track;
        }

        if ( !std::isnan( data.fix.time ) )
        {
            m_timestamp = QDateTime::fromMSecsSinceEpoch( data.fix.time * 1000 );
        }
    }
    if (m_status != oldStatus)
        emit statusChanged( m_status );
    if (!(oldPosition == m_position)) {
        emit positionChanged( m_position, m_accuracy );
    }
}

bool GpsdPositionProviderPlugin::isInitialized() const
{
    return m_thread;
}

PositionProviderPlugin* GpsdPositionProviderPlugin::newInstance() const
{
    return new GpsdPositionProviderPlugin;
}

PositionProviderStatus GpsdPositionProviderPlugin::status() const
{
    return m_status;
}

GeoDataCoordinates GpsdPositionProviderPlugin::position() const
{
    return m_position;
}

GeoDataAccuracy GpsdPositionProviderPlugin::accuracy() const
{
    return m_accuracy;
}

GpsdPositionProviderPlugin::GpsdPositionProviderPlugin() : m_thread( nullptr ),
    m_speed( 0.0 ),
    m_track( 0.0 )
{
}

GpsdPositionProviderPlugin::~GpsdPositionProviderPlugin()
{
    if ( m_thread ) {
        m_thread->exit();
        
        if ( !m_thread->wait( 5000 ) ) {
            mDebug() << "Failed to stop GpsdThread";
        }
        else {
            delete m_thread;
        }
    }
}

qreal GpsdPositionProviderPlugin::speed() const
{
    return m_speed;
}

qreal GpsdPositionProviderPlugin::direction() const
{
    return m_track;
}

QDateTime GpsdPositionProviderPlugin::timestamp() const
{
    return m_timestamp;
}


QString GpsdPositionProviderPlugin::error() const
{
    return m_thread->error();
}

#include "moc_GpsdPositionProviderPlugin.cpp"
