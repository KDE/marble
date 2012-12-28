//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011      Dennis Nienhüser <earthwings@gentoo.org>
//

#include "Tracking.h"

#include "MarbleDeclarativeWidget.h"
#include "MarbleModel.h"
#include "PositionTracking.h"
#include "RenderPlugin.h"
#include "ViewportParams.h"
#include "AutoNavigation.h"

Tracking::Tracking( QObject* parent) : QObject( parent ),
    m_showTrack( true ),
    m_positionSource( 0 ),
    m_positionMarker( 0 ),
    m_marbleWidget( 0 ),
    m_hasLastKnownPosition( false ),
    m_autoNavigation( 0 ),
    m_positionMarkerType( None )
{
    connect( &m_lastKnownPosition, SIGNAL( longitudeChanged() ), this, SLOT( setHasLastKnownPosition() ) );
    connect( &m_lastKnownPosition, SIGNAL( latitudeChanged() ), this, SLOT( setHasLastKnownPosition() ) );
}

bool Tracking::showTrack() const
{
    return m_showTrack;
}

void Tracking::setShowTrack( bool show )
{
    if ( show != m_showTrack ) {
        if ( m_marbleWidget ) {
            m_marbleWidget->model()->positionTracking()->setTrackVisible( show );
            m_marbleWidget->update();
        }

        m_showTrack = show;
        emit showTrackChanged();
    }
}

PositionSource* Tracking::positionSource()
{
    return m_positionSource;
}

void Tracking::setPositionSource( PositionSource* source )
{
    if ( source != m_positionSource ) {
        m_positionSource = source;
        if ( source ) {
            source->setMarbleModel( m_marbleWidget->model() );
            connect( source, SIGNAL( positionChanged() ),
                    this, SLOT( updatePositionMarker() ) );
            connect( source, SIGNAL( positionChanged() ),
                    this, SLOT( updateLastKnownPosition() ) );
            connect( source, SIGNAL( hasPositionChanged() ),
                    this, SLOT( updatePositionMarker() ) );
            connect( m_marbleWidget, SIGNAL( visibleLatLonAltBoxChanged() ),
                    this, SLOT( updatePositionMarker() ) );
            connect( source, SIGNAL( positionChanged() ),
                     this, SIGNAL( distanceChanged() ) );
        }
        emit positionSourceChanged();
    }
}

void Tracking::setMarbleWidget( MarbleWidget* widget )
{
    if ( widget != m_marbleWidget ) {
        if ( widget ) {
            widget->model()->positionTracking()->setTrackVisible( showTrack() );
            setShowPositionMarkerPlugin( m_positionMarkerType == Arrow );
        }

        if ( m_positionSource ) {
            m_positionSource->setMarbleModel( widget->model() );
        }

        m_marbleWidget = widget;
        connect( m_marbleWidget, SIGNAL( mapThemeChanged() ), this, SLOT( updatePositionMarker() ) );
    }
}

void Tracking::setPositionMarker( QObject* marker )
{
    if ( marker != m_positionMarker ) {
        m_positionMarker = marker;
        emit positionMarkerChanged();
    }
}

QObject* Tracking::positionMarker()
{
    return m_positionMarker;
}

void Tracking::updatePositionMarker()
{
    if ( m_marbleWidget && m_positionMarker && m_positionMarkerType == Circle ) {
        Coordinate* position = 0;
        bool visible = m_marbleWidget->model()->planetId() == "earth";
        if ( m_positionSource && m_positionSource->hasPosition() ) {
            position = m_positionSource->position();
        } else if ( hasLastKnownPosition() ) {
            position = lastKnownPosition();
        } else {
            visible = false;
        }

        qreal x(0), y(0);
        if ( position ) {
            Marble::GeoDataCoordinates const pos( position->longitude(), position->latitude(), 0.0, GeoDataCoordinates::Degree );
            visible = visible && m_marbleWidget->viewport()->screenCoordinates( pos.longitude(), pos.latitude(), x, y );
            QDeclarativeItem* item = qobject_cast<QDeclarativeItem*>( m_positionMarker );
            if ( item ) {
                item->setVisible( visible );
                if ( visible ) {
                    item->setPos( x - item->width() / 2.0, y - item->height() / 2.0 );
                }
            }
        }
    } else if ( m_positionMarkerType != Circle ) {
        QDeclarativeItem* item = qobject_cast<QDeclarativeItem*>( m_positionMarker );
        if ( item ) {
            item->setVisible( false );
        }
    }
}

void Tracking::updateLastKnownPosition()
{
    if ( m_positionSource && m_positionSource->hasPosition() ) {
        setLastKnownPosition( m_positionSource->position() );
    }
}

void Tracking::setHasLastKnownPosition()
{
    if ( !m_hasLastKnownPosition ) {
        m_hasLastKnownPosition = true;
        emit hasLastKnownPositionChanged();
    }
}

void Tracking::setShowPositionMarkerPlugin( bool visible )
{
    if ( m_marbleWidget ) {
        QList<QObject*> const renderPlugins = m_marbleWidget->renderPlugins();
        foreach( QObject* object, renderPlugins ) {
            Marble::RenderPlugin* renderPlugin = qobject_cast<Marble::RenderPlugin*>( object );
            Q_ASSERT( renderPlugin );
            if ( renderPlugin->nameId() == "positionMarker" ) {
                renderPlugin->setEnabled( true );
                renderPlugin->setVisible( visible );
            }
        }
   }
}

bool Tracking::hasLastKnownPosition() const
{
    return m_hasLastKnownPosition;
}

Coordinate * Tracking::lastKnownPosition()
{
    return &m_lastKnownPosition;
}

void Tracking::setLastKnownPosition( Coordinate* lastKnownPosition )
{
    if ( lastKnownPosition && *lastKnownPosition != m_lastKnownPosition ) {
        m_lastKnownPosition.setCoordinates( lastKnownPosition->coordinates() );
        emit lastKnownPositionChanged();
    }
}

bool Tracking::autoCenter() const
{
    if ( m_autoNavigation ) {
        return m_autoNavigation->recenterMode() != Marble::AutoNavigation::DontRecenter;
    }

    return false;
}

void Tracking::setAutoCenter( bool enabled )
{
    if ( autoCenter() != enabled ) {
        if ( enabled && !m_autoNavigation && m_marbleWidget ) {
            m_autoNavigation = new Marble::AutoNavigation( m_marbleWidget->model(), m_marbleWidget->viewport(), this );
            connect( m_autoNavigation, SIGNAL( zoomIn( FlyToMode ) ),
                     m_marbleWidget, SLOT( zoomIn() ) );
            connect( m_autoNavigation, SIGNAL( zoomOut( FlyToMode ) ),
                     m_marbleWidget, SLOT( zoomOut() ) );
            connect( m_autoNavigation, SIGNAL( centerOn( const GeoDataCoordinates &, bool ) ),
                     m_marbleWidget, SLOT( centerOn( const GeoDataCoordinates & ) ) );

            connect( m_marbleWidget, SIGNAL( visibleLatLonAltBoxChanged() ),
                     m_autoNavigation, SLOT( inhibitAutoAdjustments() ) );
        }

        if ( m_autoNavigation ) {
            m_autoNavigation->setRecenter( Marble::AutoNavigation::RecenterOnBorder );
        }

        emit autoCenterChanged();
    }
}

bool Tracking::autoZoom() const
{
    if ( m_autoNavigation ) {
        return m_autoNavigation->autoZoom();
    }

    return false;
}

void Tracking::setAutoZoom( bool enabled )
{
    if ( autoZoom() != enabled ) {
        if ( enabled && !m_autoNavigation && m_marbleWidget ) {
            m_autoNavigation = new Marble::AutoNavigation( m_marbleWidget->model(), m_marbleWidget->viewport(), this );
            connect( m_autoNavigation, SIGNAL( zoomIn( FlyToMode ) ),
                     m_marbleWidget, SLOT( zoomIn() ) );
            connect( m_autoNavigation, SIGNAL( zoomOut( FlyToMode ) ),
                     m_marbleWidget, SLOT( zoomOut() ) );
            connect( m_autoNavigation, SIGNAL( centerOn( const GeoDataCoordinates &, bool ) ),
                     m_marbleWidget, SLOT( centerOn( const GeoDataCoordinates & ) ) );

            connect( m_marbleWidget, SIGNAL( visibleLatLonAltBoxChanged() ),
                     m_autoNavigation, SLOT( inhibitAutoAdjustments() ) );
        }

        if ( m_autoNavigation ) {
            m_autoNavigation->setAutoZoom( enabled );
        }

        emit autoZoomChanged();
    }
}

Tracking::PositionMarkerType Tracking::positionMarkerType() const
{
    return m_positionMarkerType;
}

void Tracking::setPositionMarkerType( Tracking::PositionMarkerType type )
{
    setShowPositionMarkerPlugin( type == Arrow );
    if ( type != m_positionMarkerType ) {
        m_positionMarkerType = type;
        emit positionMarkerTypeChanged();
    }
}

double Tracking::distance() const
{
    return m_marbleWidget ? m_marbleWidget->model()->positionTracking()->length( m_marbleWidget->model()->planetRadius() ) : 0.0;
}

void Tracking::saveTrack( const QString &fileName )
{
    if ( m_marbleWidget ) {
        /** @todo FIXME: replace the file:// prefix on QML side */
        QString target = fileName.startsWith( QLatin1String( "file://" ) ) ? fileName.mid( 7 ) : fileName;
        m_marbleWidget->model()->positionTracking()->saveTrack( target );
    }
}

void Tracking::openTrack(const QString &fileName)
{
    if ( m_marbleWidget ) {
        /** @todo FIXME: replace the file:// prefix on QML side */
        QString target = fileName.startsWith( QLatin1String( "file://" ) ) ? fileName.mid( 7 ) : fileName;
        m_marbleWidget->model()->addGeoDataFile( target );
    }
}

void Tracking::clearTrack()
{
    if ( m_marbleWidget ) {
        m_marbleWidget->model()->positionTracking()->clearTrack();
    }
}

#include "Tracking.moc"
