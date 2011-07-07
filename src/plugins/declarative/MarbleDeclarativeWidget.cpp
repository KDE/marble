//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010      Dennis Nienhüser <earthwings@gentoo.org>
//

#include "MarbleDeclarativeWidget.h"

#include "MapTheme.h"
#include "Coordinate.h"
#include "Tracking.h"

#include "GeoDataCoordinates.h"
#include "MarbleWidget.h"
#include "MarbleWidgetInputHandler.h"
#include "MarbleMath.h"
#include "AbstractFloatItem.h"
#include "MarbleMap.h"
#include "ViewParams.h"
#include "ViewportParams.h"
#include "AbstractProjection.h"

namespace Marble
{
namespace Declarative
{

MarbleWidget::MarbleWidget( QGraphicsItem *parent , Qt::WindowFlags flags ) :
    QGraphicsProxyWidget( parent, flags ), m_marbleWidget( new Marble::MarbleWidget ),
    m_inputEnabled( true ), m_tracking( 0 ), m_routing( 0 )
{
    m_marbleWidget->setMapThemeId( "earth/openstreetmap/openstreetmap.dgml" );
    setWidget( m_marbleWidget );

    connect( m_marbleWidget, SIGNAL( visibleLatLonAltBoxChanged( GeoDataLatLonAltBox ) ),
             this, SIGNAL( visibleLatLonAltBoxChanged( ) ) );
    connect( &m_center, SIGNAL(latitudeChanged()), this, SLOT(updateCenterPosition()));
    connect( &m_center, SIGNAL(longitudeChanged()), this, SLOT(updateCenterPosition()));

    m_marbleWidget->inputHandler()->setMouseButtonPopupEnabled( Qt::LeftButton, false );
}

QStringList MarbleWidget::activeFloatItems() const
{
    QStringList result;
    foreach( AbstractFloatItem * floatItem, m_marbleWidget->floatItems() ) {
        if ( floatItem->enabled() && floatItem->visible() ) {
            result << floatItem->nameId();
        }
    }
    return result;
}

void MarbleWidget::setActiveFloatItems( const QStringList &items )
{
    foreach( AbstractFloatItem * floatItem, m_marbleWidget->floatItems() ) {
        floatItem->setEnabled( items.contains( floatItem->nameId() ) );
        floatItem->setVisible( items.contains( floatItem->nameId() ) );
    }
}

bool MarbleWidget::inputEnabled() const
{
    return m_inputEnabled;
}

void MarbleWidget::setInputEnabled( bool enabled )
{
    m_inputEnabled = enabled;
    m_marbleWidget->setInputEnabled( enabled );
}

QString MarbleWidget::mapThemeId() const
{
    return m_marbleWidget->mapThemeId();
}

void MarbleWidget::setMapThemeId( const QString &mapThemeId )
{
    m_marbleWidget->setMapThemeId( mapThemeId );
}

QString MarbleWidget::projection( ) const
{
    switch ( m_marbleWidget->projection() ) {
    case Equirectangular:
        return "Equirectangular";
    case Mercator:
        return "Mercator";
    case Spherical:
        return "Spherical";
    }

    Q_ASSERT( false && "Marble got a new projection which we do not know about yet" );
    return "Spherical";
}

void MarbleWidget::setProjection( const QString &projection )
{
    if ( projection.compare( "Equirectangular", Qt::CaseInsensitive ) == 0 ) {
        m_marbleWidget->setProjection( Equirectangular );
    } else if ( projection.compare( "Mercator", Qt::CaseInsensitive ) == 0 ) {
        m_marbleWidget->setProjection( Mercator );
    } else {
        m_marbleWidget->setProjection( Spherical );
    }
}

void MarbleWidget::zoomIn()
{
    m_marbleWidget->zoomIn();
}

void MarbleWidget::zoomOut()
{
    m_marbleWidget->zoomOut();
}

QPoint MarbleWidget::pixel( qreal lon, qreal lat ) const
{
    GeoDataCoordinates position( lon, lat, 0, GeoDataCoordinates::Degree );
    qreal x( 0.0 );
    qreal y( 0.0 );
    ViewportParams *viewport = m_marbleWidget->viewport();
    bool hidden = false;
    QPoint result;
    if ( viewport->currentProjection()->screenCoordinates( position, viewport, x, y, hidden ) && !hidden ) {
        result = QPoint( x, y );
    }
    return result;
}

Coordinate *MarbleWidget::coordinate( int x, int y )
{
    qreal lat( 0.0 ), lon( 0.0 );
    m_marbleWidget->geoCoordinates( x, y, lon, lat );
    return new Coordinate( lon, lat, 0.0, this );
}

Marble::Declarative::Tracking* MarbleWidget::tracking()
{
    if ( !m_tracking ) {
        m_tracking = new Tracking( this );
        m_tracking->setMarbleWidget( m_marbleWidget );
        emit trackingChanged();
    }

    return m_tracking;
}

Coordinate* MarbleWidget::center()
{
    return &m_center;
}

void MarbleWidget::setCenter( Coordinate* center )
{
    if ( center ) {
        m_center.setLongitude( center->longitude() );
        m_center.setLatitude( center->latitude() );
        m_center.setAltitude( center->altitude() );
        updateCenterPosition();
    }
}

void MarbleWidget::updateCenterPosition()
{
  m_marbleWidget->centerOn( m_center.longitude(), m_center.latitude() );
  emit centerChanged();
}

Marble::Declarative::Routing* MarbleWidget::routing()
{
    if ( !m_routing ) {
        m_routing = new Routing( this );
        m_routing->setMarbleWidget( m_marbleWidget );
    }

    return m_routing;
}

}
}

#include "MarbleDeclarativeWidget.moc"
