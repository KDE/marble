//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010      Dennis Nienhüser <earthwings@gentoo.org>
//

#include "MarbleDeclarativeWidget.h"

#include "MapTheme.h"

#include "GeoDataCoordinates.h"
#include "MarbleWidget.h"
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
    m_inputEnabled( true )
{
    m_marbleWidget->setMapThemeId( "earth/openstreetmap/openstreetmap.dgml" );
    setWidget( m_marbleWidget );

    connect( m_marbleWidget, SIGNAL( visibleLatLonAltBoxChanged( GeoDataLatLonAltBox ) ),
             this, SIGNAL( visibleLatLonAltBoxChanged( ) ) );
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
    if ( projection.compare( "Equirectangular", Qt::CaseInsensitive ) ) {
        m_marbleWidget->setProjection( Equirectangular );
    } else if ( projection.compare( "Mercator", Qt::CaseInsensitive ) ) {
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

qreal MarbleWidget::screenX( qreal lon, qreal lat ) const
{
    GeoDataCoordinates position( lon, lat, 0, GeoDataCoordinates::Degree );
    qreal result( 0.0 );
    qreal y( 0.0 );
    ViewportParams *viewport = m_marbleWidget->viewport();
    bool hidden = false;
    if ( !viewport->currentProjection()->screenCoordinates( position, viewport, result, y, hidden ) || hidden ) {
        result = 0.0;
    }
    return result;
}

qreal MarbleWidget::screenY( qreal lon, qreal lat ) const
{
    GeoDataCoordinates position( lon, lat, 0, GeoDataCoordinates::Degree );
    qreal x( 0.0 );
    qreal result( 0.0 );
    ViewportParams *viewport = m_marbleWidget->viewport();
    bool hidden = false;
    if ( !viewport->currentProjection()->screenCoordinates( position, viewport, x, result, hidden ) || hidden ) {
        result = 0.0;
    }
    return result;
}

qreal MarbleWidget::lon( int x, int y ) const
{
    qreal result( 0.0 );
    qreal lat( 0.0 );
    m_marbleWidget->geoCoordinates( x, y, result, lat );
    return result;
}

qreal MarbleWidget::lat( int x, int y ) const
{
    qreal lon( 0.0 );
    qreal result( 0.0 );
    m_marbleWidget->geoCoordinates( x, y, lon, result );
    return result;
}

qreal MarbleWidget::distance( qreal flon, qreal flat, qreal tlon, qreal tlat )
{
    GeoDataCoordinates::Unit deg = GeoDataCoordinates::Degree;
    return EARTH_RADIUS * distanceSphere( GeoDataCoordinates( flon, flat, 0, deg ),
                                          GeoDataCoordinates( tlon, tlat, 0, deg ) );
}

}
}

#include "MarbleDeclarativeWidget.moc"
