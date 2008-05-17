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
#include <QtGui/QPixmap>
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

MarbleOverviewMap::~MarbleOverviewMap()
{
    delete m_svgobj;
}

QStringList MarbleOverviewMap::backendTypes() const
{
    return QStringList( "overviewmap" );
}

QString MarbleOverviewMap::name() const
{
    return tr("Overview Map");
}

QString MarbleOverviewMap::guiString() const
{
    return tr("&Overview Map");
}

QString MarbleOverviewMap::nameId() const
{
    return QString( "overviewmap" );
}

QString MarbleOverviewMap::description() const
{
    return tr("This is a float item that provides an overview map.");
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

bool MarbleOverviewMap::needsUpdate( ViewportParams *viewport )
{
    GeoDataLatLonAltBox latLonAltBox = viewport->currentProjection()->latLonAltBox( QRect( QPoint( 0, 0 ), viewport->size() ), viewport );

    if ( m_latLonAltBox == latLonAltBox ) {
//        qDebug() << "false";
        return false;
    }
    m_latLonAltBox = latLonAltBox;
//    qDebug() << "true";
    return true;
}

bool MarbleOverviewMap::renderFloatItem( GeoPainter *painter, ViewportParams *viewport, GeoSceneLayer * layer )
{
    painter->save();

    painter->autoMapQuality();

    QRectF mapRect( contentRect() );

    // Rerender worldmap pixmap if the size has changed
    if ( m_worldmap.size() != mapRect.size().toSize() ) {
        m_worldmap = QPixmap( mapRect.size().toSize() );
        m_worldmap.fill( Qt::transparent );
        QPainter mapPainter;
        mapPainter.begin(&m_worldmap);
        mapPainter.setViewport( m_worldmap.rect() );
        m_svgobj->render( &mapPainter );
        mapPainter.end(); 
    }
    painter->drawPixmap( QPoint( 0, 0 ), m_worldmap );

    // Now draw the latitude longitude bounding box
    double xWest = mapRect.width() / 2.0 
                    + mapRect.width() / ( 2.0 * M_PI ) * m_latLonAltBox.west();
    double xEast = mapRect.width() / 2.0
                    + mapRect.width() / ( 2.0 * M_PI ) * m_latLonAltBox.east();
    double xNorth = mapRect.height() / 2.0 
                    - mapRect.height() / M_PI * m_latLonAltBox.north();
    double xSouth = mapRect.height() / 2.0
                    - mapRect.height() / M_PI * m_latLonAltBox.south();

    painter->setPen( QPen( Qt::white ) );
    painter->setBrush( QBrush( Qt::transparent ) );
    painter->setRenderHint( QPainter::Antialiasing, false );

    double boxWidth  = xEast  - xWest;
    double boxHeight = xSouth - xNorth;

    double minBoxSize = 2.0;
    if ( boxHeight < minBoxSize ) boxHeight = minBoxSize;

    if ( m_latLonAltBox.west() <= m_latLonAltBox.east() ) {
        // Make sure the latLonBox is still visible
        if ( boxWidth  < minBoxSize ) boxWidth  = minBoxSize;

        painter->drawRect( QRectF( xWest, xNorth, boxWidth, boxHeight ) );
    }
    else {
        // If the dateline is shown in the viewport  and if the poles are not 
        // then there are two boxes that represent the latLonBox of the view.

        boxWidth = xEast;

        // Make sure the latLonBox is still visible
        if ( boxWidth  < minBoxSize ) boxWidth  = minBoxSize;

        painter->drawRect( QRectF( 0, xNorth, boxWidth, boxHeight ) );

        boxWidth = mapRect.width() - xWest;

        // Make sure the latLonBox is still visible
        if ( boxWidth  < minBoxSize ) boxWidth  = minBoxSize;

        painter->drawRect( QRectF( xWest, xNorth, boxWidth, boxHeight ) );
    }
    painter->restore();

    return true;
}

Q_EXPORT_PLUGIN2(MarbleOverviewMap, MarbleOverviewMap)

#include "MarbleOverviewMap.moc"
