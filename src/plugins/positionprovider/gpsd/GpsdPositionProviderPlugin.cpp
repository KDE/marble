//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009      Eckhart Wörner <ewoerner@kde.org>
//

#include "GpsdPositionProviderPlugin.h"

#include <QtCore/QDebug>



using namespace Marble;

QString GpsdPositionProviderPlugin::name() const
{
    return tr( "Gpsd position provider Plugin" );
}

QString GpsdPositionProviderPlugin::nameId() const
{
    return QString::fromLatin1( "Gpsd" );
}

QString GpsdPositionProviderPlugin::guiString() const
{
    return tr( "Gpsd position provider plugin" );
}

QString GpsdPositionProviderPlugin::description() const
{
    return tr( "Reports the the position of a GPS device." );
}

QIcon GpsdPositionProviderPlugin::icon() const
{
    return QIcon();
}

void GpsdPositionProviderPlugin::initialize()
{
    m_gpsd = new gpsmm;
    m_gpsdData = m_gpsd->open();
    m_timer = new QTimer;
    connect( m_timer, SIGNAL( timeout() ), this, SLOT( update() ) );
    m_timer->start( 1000 );
    m_initialized = true;
}

void GpsdPositionProviderPlugin::update() {
    //Check that m_gpsd->open() has successfully completed
    if ( m_gpsdData != 0 ) {
        m_gpsdData = m_gpsd->query( "o" );
        PositionProviderStatus oldStatus = m_status;
        GeoDataCoordinates oldPosition = m_position;
        if (m_gpsdData->status == STATUS_NO_FIX)
            m_status = PositionProviderStatusUnavailable;
        else {
            m_status = PositionProviderStatusAvailable;
            m_position.set( m_gpsdData->fix.longitude, m_gpsdData->fix.latitude,
            m_gpsdData->fix.altitude, GeoDataCoordinates::Degree );
            m_accuracy.level = GeoDataAccuracy::Detailed;
            // FIXME: Add real values here
            m_accuracy.horizontal = 5;
            m_accuracy.vertical = 5;
        }
        if (m_status != oldStatus)
            emit statusChanged( m_status );
        // FIXME: Check whether position has changed first
        if (m_status == PositionProviderStatusAvailable)
            emit positionChanged( m_position, m_accuracy );
    }
}

bool GpsdPositionProviderPlugin::isInitialized() const
{
    return m_initialized;
}

PositionProviderPlugin* GpsdPositionProviderPlugin::newInstance() const
{
    return new GpsdPositionProviderPlugin;
}

PositionProviderStatus GpsdPositionProviderPlugin::status() const {
    return m_status;
}

GeoDataCoordinates GpsdPositionProviderPlugin::position() const {
    return m_position;
}

GeoDataAccuracy GpsdPositionProviderPlugin::accuracy() const {
    return m_accuracy;
}

GpsdPositionProviderPlugin::GpsdPositionProviderPlugin() : m_initialized(false) {}

GpsdPositionProviderPlugin::~GpsdPositionProviderPlugin()
{
    if (m_initialized) {
        delete m_timer;
        delete m_gpsd;
    }
}

Q_EXPORT_PLUGIN2( GpsdPositionProviderPlugin, Marble::GpsdPositionProviderPlugin )



#include "GpsdPositionProviderPlugin.moc"
