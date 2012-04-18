//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011      Dennis Nienhüser <earthwings@gentoo.org>
// Copyright 2011      Bernhard Beschow <bbeschow@cs.tu-berlin.de>
//

#include "PositionSource.h"

#include "MarbleModel.h"
#include "PluginManager.h"
#include "PositionTracking.h"
#include "PositionProviderPlugin.h"

PositionSource::PositionSource( QObject* parent) : QObject( parent ),
    m_active( false ),
    m_hasPosition( false ),
    m_position( 0 ),
    m_marbleModel( 0 ),
    m_speed( 0.0 )
{
  // nothing to do
}

bool PositionSource::active() const
{
    return m_active;
}

void PositionSource::setActive( bool active )
{
    if ( active != m_active ) {
        if ( active ) {
            start();
        } else if ( m_marbleModel ) {
            Marble::PositionTracking *tracking = m_marbleModel->positionTracking();
            tracking->setPositionProviderPlugin( 0 );
        }

        if ( m_hasPosition ) {
            m_hasPosition = false;
            emit hasPositionChanged();
        }

        m_active = active;
        emit activeChanged();
    }
}

QString PositionSource::source() const
{
    return m_source;
}

void PositionSource::setSource( const QString &source )
{
    if ( source != m_source ) {
        m_source = source;
        if ( m_hasPosition ) {
            m_hasPosition = false;
            emit hasPositionChanged();
        }

        if ( active() ) {
            start();
        }
        emit sourceChanged();
    }
}

bool PositionSource::hasPosition() const
{
    return m_hasPosition;
}

Coordinate* PositionSource::position()
{
    return &m_position;
}

void PositionSource::start()
{
    if ( !m_marbleModel ) {
        return;
    }

    const Marble::PluginManager* pluginManager = m_marbleModel->pluginManager();
    foreach( const Marble::PositionProviderPlugin *plugin, pluginManager->positionProviderPlugins() ) {
        if ( m_source.isEmpty() || plugin->nameId() == m_source ) {
            Marble::PositionProviderPlugin* instance = plugin->newInstance();
            instance->setMarbleModel( m_marbleModel );
            Marble::PositionTracking *tracking = m_marbleModel->positionTracking();
            tracking->setPositionProviderPlugin( instance );
            break;
        }
    }
}

void PositionSource::setMarbleModel( Marble::MarbleModel* model )
{
    if ( model != m_marbleModel ) {
        m_marbleModel = model;

        if ( model ) {
            connect( model->positionTracking(), SIGNAL( gpsLocation( GeoDataCoordinates, qreal ) ),
                    this, SLOT( updatePosition() ) );
            connect( model->positionTracking(), SIGNAL( statusChanged( PositionProviderStatus ) ),
                    this, SLOT( updatePosition() ) );
        }

        if ( active() ) {
            start();
        }
    }
}

qreal PositionSource::speed() const
{
    return m_speed;
}

void PositionSource::updatePosition()
{
    if ( m_marbleModel ) {
        bool const hasPosition = m_marbleModel->positionTracking()->status() == Marble::PositionProviderStatusAvailable;

        if ( hasPosition ) {
            Marble::GeoDataCoordinates position = m_marbleModel->positionTracking()->currentLocation();
            m_position.setLongitude( position.longitude( Marble::GeoDataCoordinates::Degree ) );
            m_position.setLatitude( position.latitude( Marble::GeoDataCoordinates::Degree ) );
            m_position.setAltitude( position.altitude() );
        }

        m_speed = m_marbleModel->positionTracking()->speed() * Marble::METER2KM / Marble::SEC2HOUR;
        emit speedChanged();

        if ( hasPosition != m_hasPosition ) {
            m_hasPosition = hasPosition;
            emit hasPositionChanged();
        }

        if ( hasPosition ) {
            emit positionChanged();
        }
    }
}

#include "PositionSource.moc"
