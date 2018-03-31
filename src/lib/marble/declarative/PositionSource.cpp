//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011      Dennis Nienhüser <nienhueser@kde.org>
// Copyright 2011      Bernhard Beschow <bbeschow@cs.tu-berlin.de>
//

#include "PositionSource.h"

#include "MarbleQuickItem.h"
#include "MarbleModel.h"
#include "PluginManager.h"
#include "PositionTracking.h"
#include "PositionProviderPlugin.h"

namespace Marble {

PositionSource::PositionSource( QObject* parent) : QObject( parent ),
    m_active( false ),
    m_hasPosition( false ),
    m_position( 0 ),
    m_marbleQuickItem( nullptr ),
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
        } else if ( m_marbleQuickItem ) {
            PositionTracking *tracking = m_marbleQuickItem->model()->positionTracking();
            tracking->setPositionProviderPlugin( nullptr );
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
    if ( !m_marbleQuickItem ) {
        return;
    }

    const PluginManager* pluginManager = m_marbleQuickItem->model()->pluginManager();
    for( const Marble::PositionProviderPlugin *plugin: pluginManager->positionProviderPlugins() ) {
        if ( m_source.isEmpty() || plugin->nameId() == m_source ) {
            PositionProviderPlugin* instance = plugin->newInstance();
            PositionTracking *tracking = m_marbleQuickItem->model()->positionTracking();
            tracking->setPositionProviderPlugin( instance );
            break;
        }
    }
}

MarbleQuickItem *PositionSource::map()
{
    return m_marbleQuickItem;
}

void PositionSource::setMap( MarbleQuickItem *map )
{
    if ( map != m_marbleQuickItem ) {
        m_marbleQuickItem = map;

        if ( m_marbleQuickItem ) {
            connect( m_marbleQuickItem->model()->positionTracking(), SIGNAL(gpsLocation(GeoDataCoordinates,qreal)),
                    this, SLOT(updatePosition()) );
            connect( m_marbleQuickItem->model()->positionTracking(), SIGNAL(statusChanged(PositionProviderStatus)),
                    this, SLOT(updatePosition()) );

            emit mapChanged();
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
    if ( m_marbleQuickItem ) {
        bool const hasPosition = m_marbleQuickItem->model()->positionTracking()->status() == Marble::PositionProviderStatusAvailable;

        if ( hasPosition ) {
            Marble::GeoDataCoordinates position = m_marbleQuickItem->model()->positionTracking()->currentLocation();
            m_position.setLongitude( position.longitude( Marble::GeoDataCoordinates::Degree ) );
            m_position.setLatitude( position.latitude( Marble::GeoDataCoordinates::Degree ) );
            m_position.setAltitude( position.altitude() );
        }

        m_speed = m_marbleQuickItem->model()->positionTracking()->speed() * Marble::METER2KM / Marble::SEC2HOUR;
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

}

#include "moc_PositionSource.cpp"
