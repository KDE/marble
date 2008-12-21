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
#include <QtGui/QCursor>

#include "AbstractProjection.h"
#include "MarbleDirs.h"
#include "MarbleDataFacade.h"

#include "GeoPainter.h"
#include "GeoDataPoint.h"
#include "ViewportParams.h"
#include "MarbleWidget.h"
#include "MarbleMap.h"

namespace Marble
{

MarbleOverviewMap::MarbleOverviewMap( const QPointF &point, const QSizeF &size )
    : MarbleAbstractFloatItem( point, size ),
      m_target(QString()),
      m_svgobj(0)
{
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
    qreal centerLon, centerLat;
    viewport->centerCoordinates( centerLon, centerLat );
    QString target = dataFacade()->target();

    if ( m_latLonAltBox == latLonAltBox 
	&& m_centerLon == centerLon
	&& m_centerLat == centerLat 
    && m_target == target ) {
        return false;
    }
    m_latLonAltBox = latLonAltBox;
    m_centerLon = centerLon;
    m_centerLat = centerLat;
//    qDebug() << "true";
    return true;
}

bool MarbleOverviewMap::renderFloatItem( GeoPainter *painter, ViewportParams *viewport, GeoSceneLayer * layer )
{
    Q_UNUSED(layer);
    painter->save();

    painter->autoMapQuality();

    QRectF mapRect( contentRect() );

    QString target = dataFacade()->target();

    if ( target != m_target ) {
        changeBackground( target );
    }

    // Rerender worldmap pixmap if the size has changed
    if (    m_worldmap.size() != mapRect.size().toSize() 
         || target != m_target ) {

        m_worldmap = QPixmap( mapRect.size().toSize() );
        m_worldmap.fill( Qt::transparent );
        QPainter mapPainter;
        mapPainter.begin(&m_worldmap);
        mapPainter.setViewport( m_worldmap.rect() );
        m_svgobj->render( &mapPainter );
        mapPainter.end(); 

        m_target = target;
    }

    painter->drawPixmap( QPoint( 0, 0 ), m_worldmap );

    // Now draw the latitude longitude bounding box
    qreal xWest = mapRect.width() / 2.0 
                    + mapRect.width() / ( 2.0 * M_PI ) * m_latLonAltBox.west();
    qreal xEast = mapRect.width() / 2.0
                    + mapRect.width() / ( 2.0 * M_PI ) * m_latLonAltBox.east();
    qreal xNorth = mapRect.height() / 2.0 
                    - mapRect.height() / M_PI * m_latLonAltBox.north();
    qreal xSouth = mapRect.height() / 2.0
                    - mapRect.height() / M_PI * m_latLonAltBox.south();

    qreal lon, lat;
    viewport->centerCoordinates( lon, lat );
    GeoDataPoint::normalizeLonLat( lon, lat );
    qreal x = mapRect.width() / 2.0 + mapRect.width() / ( 2.0 * M_PI ) * lon;
    qreal y = mapRect.height() / 2.0 - mapRect.height() / M_PI * lat;

    painter->setPen( QPen( Qt::white ) );
    painter->setBrush( QBrush( Qt::transparent ) );
    painter->setRenderHint( QPainter::Antialiasing, false );

    qreal boxWidth  = xEast  - xWest;
    qreal boxHeight = xSouth - xNorth;

    qreal minBoxSize = 2.0;
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

    painter->setPen( QPen( Qt::white ) );
    painter->setBrush( QBrush( Qt::white ) );

    qreal circleRadius = 2.5;
    painter->setRenderHint( QPainter::Antialiasing, true );
    painter->drawEllipse( QRectF( x - circleRadius, y - circleRadius , 2 * circleRadius, 2 * circleRadius ) );

    painter->restore();

    return true;
}

bool MarbleOverviewMap::eventFilter( QObject *object, QEvent *e )
{
    if ( !enabled() || !visible() ) {
        return false;
    }

    MarbleWidget *widget = dynamic_cast<MarbleWidget*>(object);
    if ( !widget ) {
        return MarbleAbstractFloatItem::eventFilter(object,e);
    }

    bool cursorAboveFloatItem(false);
    if ( e->type() == QEvent::MouseButtonDblClick || e->type() == QEvent::MouseMove ) {
        QMouseEvent *event = static_cast<QMouseEvent*>(e);
        QRectF floatItemRect = QRectF(positivePosition(QRectF(0,0,widget->width(),
                widget->height())), size());

        if ( floatItemRect.contains(event->pos()) ) {
            cursorAboveFloatItem = true;

            // Double click triggers recentering the map at the specified position
            if ( e->type() == QEvent::MouseButtonDblClick ) {
                QRectF mapRect( contentRect() );
                QPointF pos = event->pos() - floatItemRect.topLeft() 
                    - QPointF(padding(),padding());

                qreal lon = ( pos.x() - mapRect.width() / 2.0 ) / mapRect.width() * 360.0 ;
                qreal lat = ( mapRect.height() / 2.0 - pos.y() ) / mapRect.height() * 180.0;
                widget->centerOn(lon,lat,true);

                return true;
            }
        }

        if ( cursorAboveFloatItem && e->type() == QEvent::MouseMove 
                && !event->buttons() & Qt::LeftButton )
        {
            // Cross hair cursor when moving above the float item without pressing a button
            widget->setCursor(QCursor(Qt::CrossCursor));
            return true;
        }
    }

    return MarbleAbstractFloatItem::eventFilter(object,e);
}

void MarbleOverviewMap::changeBackground( const QString& target ) {

    delete m_svgobj;

    if ( target == "moon" ) {
        m_svgobj = new QSvgRenderer( MarbleDirs::path( "svg/lunarmap.svg" ),
                                    this );        
        return;
    }

    m_svgobj = new QSvgRenderer( MarbleDirs::path( "svg/worldmap.svg" ),
                                 this );
}

}

Q_EXPORT_PLUGIN2(MarbleOverviewMap, Marble::MarbleOverviewMap)

#include "MarbleOverviewMap.moc"
