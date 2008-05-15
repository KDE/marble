//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2008 Torsten Rahn <tackat@kde.org>"
//

#include "MarbleOverviewMap.h"

#include <QtCore/QRect>
#include <QtGui/QColor>
#include <QtGui/QPixmap>
#include <QtGui/QRadialGradient>
#include <QtSvg/QSvgRenderer>

#include "MarbleDirs.h"
#include "GeoPainter.h"
#include "GeoDataPoint.h"
#include "ViewportParams.h"


MarbleOverviewMap::MarbleOverviewMap( const QPointF &point, const QSizeF &size )
    : MarbleAbstractFloatItem( point, size )
{
    m_svgobj = new QSvgRenderer( MarbleDirs::path( "svg/worldmap.svg" ),
                                 this );
}

QStringList MarbleOverviewMap::backendTypes() const
{
    return QStringList( "overviewmap" );
}

QString MarbleOverviewMap::name() const
{
    return QString( "Overview Map" );
}

QString MarbleOverviewMap::nameId() const
{
    return QString( "overviewmap-plugin" );
}

QString MarbleOverviewMap::description() const
{
    return QString( "This is a float item that provides an overview map." );
}

QIcon MarbleOverviewMap::icon () const
{
    return QIcon();
}


void MarbleOverviewMap::initialize ()
{
}

bool MarbleOverviewMap::isInitialized () const
{
    return true;
}

bool MarbleOverviewMap::renderContent( GeoPainter *painter, ViewportParams *viewport, GeoSceneLayer * layer )
{
    painter->autoMapQuality();

    QRectF mapRect( contentRect() );

    painter->setViewport( mapRect.toRect() );
    m_svgobj->render( painter ); 
    painter->setViewport( QRect( QPoint( 0, 0 ), viewport->size() ) );
    GeoDataLatLonAltBox latLonAltBox = viewport->currentProjection()->latLonAltBox( QRect( QPoint( 0, 0 ), viewport->size() ), viewport );

    double xWest = mapRect.width() / 2 
                    + mapRect.width() / ( 2 * M_PI ) * latLonAltBox.west();
    double xEast = mapRect.width() / 2
                    + mapRect.width() / ( 2 * M_PI ) * latLonAltBox.east();
    double xNorth = mapRect.height() / 2 
                    - mapRect.height() / M_PI * latLonAltBox.north();
    double xSouth = mapRect.height() / 2
                    - mapRect.height() / M_PI * latLonAltBox.south();

    painter->setPen( QPen( Qt::white ) );
    painter->setBrush( QBrush( Qt::transparent ) );
    painter->setRenderHint( QPainter::Antialiasing, false );

    double boxWidth  = xEast  - xWest;
    double boxHeight = xSouth - xNorth;

    double minBoxSize = 2.0;

    if ( latLonAltBox.west() <= latLonAltBox.east() ) {
        // Make sure the latLonBox is still visible
        if ( boxWidth  < minBoxSize ) boxWidth  = minBoxSize;
        if ( boxHeight < minBoxSize ) boxHeight = minBoxSize;

        painter->drawRect( QRectF( xWest + mapRect.left(), xNorth + mapRect.top(), boxWidth, boxHeight ) );
    }
    else {
        // If the dateline is shown in the viewport  and if the poles are not 
        // then there are two boxes that represent the latLonBox of the view.

        boxWidth = xEast;

        // Make sure the latLonBox is still visible
        if ( boxWidth  < minBoxSize ) boxWidth  = minBoxSize;
        if ( boxHeight < minBoxSize ) boxHeight = minBoxSize;

        painter->drawRect( QRectF( mapRect.left(), xNorth + mapRect.top(), boxWidth, boxHeight ) );

        boxWidth = mapRect.width() - xWest;

        // Make sure the latLonBox is still visible
        if ( boxWidth  < minBoxSize ) boxWidth  = minBoxSize;
        if ( boxHeight < minBoxSize ) boxHeight = minBoxSize;

        painter->drawRect( QRectF( mapRect.left() + xWest, xNorth + mapRect.top(), mapRect.width() - xWest, boxHeight ) );
    }
    return true;
}

Q_EXPORT_PLUGIN2(MarbleOverviewMap, MarbleOverviewMap)

#include "MarbleOverviewMap.moc"
