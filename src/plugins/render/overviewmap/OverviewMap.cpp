//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2008 Torsten Rahn <tackat@kde.org>
//

#include "OverviewMap.h"

#include <QtCore/QRect>
#include <QtGui/QCursor>
#include <QtGui/QMouseEvent>
#include <QtGui/QPixmap>
#include <QtSvg/QSvgRenderer>

#include "AbstractProjection.h"
#include "MarbleDirs.h"
#include "MarbleDataFacade.h"

#include "GeoPainter.h"
#include "GeoDataPoint.h"
#include "ViewportParams.h"
#include "MarbleWidget.h"

namespace Marble
{

OverviewMap::OverviewMap( const QPointF &point, const QSizeF &size )
    : AbstractFloatItem( point, size ),
      m_target(QString()),
      m_svgobj(0)
{
    // cache is no needed because:
    // (1) the SVG overview map is already rendered and stored in m_worldmap pixmap
    // (2) bounding box and location dot keep changing during navigation
    setCacheMode( NoCache );
}

OverviewMap::~OverviewMap()
{
    delete m_svgobj;
}

QStringList OverviewMap::backendTypes() const
{
    return QStringList( "overviewmap" );
}

QString OverviewMap::name() const
{
    return tr("Overview Map");
}

QString OverviewMap::guiString() const
{
    return tr("&Overview Map");
}

QString OverviewMap::nameId() const
{
    return QString( "overviewmap" );
}

QString OverviewMap::description() const
{
    return tr("This is a float item that provides an overview map.");
}

QIcon OverviewMap::icon () const
{
    return QIcon();
}


void OverviewMap::initialize ()
{
}

bool OverviewMap::isInitialized () const
{
    return true;
}

void OverviewMap::changeViewport( ViewportParams *viewport )
{
    GeoDataLatLonAltBox latLonAltBox = viewport->currentProjection()->latLonAltBox( QRect( QPoint( 0, 0 ), viewport->size() ), viewport );
    qreal centerLon, centerLat;
    viewport->centerCoordinates( centerLon, centerLat );
    QString target = dataFacade()->target();

    if ( !( m_latLonAltBox == latLonAltBox
            && m_centerLon == centerLon
            && m_centerLat == centerLat
            && m_target == target ) )
    {
        m_latLonAltBox = latLonAltBox;
        m_centerLon = centerLon;
        m_centerLat = centerLat;
        update();
    }
}

void OverviewMap::paintContent( GeoPainter *painter, ViewportParams *viewport,
                                const QString& renderPos, GeoSceneLayer * layer )
{
    Q_UNUSED( layer );
    Q_UNUSED( renderPos );

    painter->save();

    painter->autoMapQuality();

    QRectF mapRect( contentRect() );

    QString target = dataFacade()->target();

    if ( target != m_target ) {
        changeBackground( target );
    }

    if ( m_svgobj ) {
        // Rerender worldmap pixmap if the size has changed
        if ( m_worldmap.size() != mapRect.size().toSize() 
            || target != m_target ) {

            m_worldmap = QPixmap( mapRect.size().toSize() );
            m_worldmap.fill( Qt::transparent );
            QPainter mapPainter;
            mapPainter.begin(&m_worldmap);
            mapPainter.setViewport( m_worldmap.rect() );
            m_svgobj->render( &mapPainter );
            mapPainter.end(); 
        }

        painter->drawPixmap( QPoint( 0, 0 ), m_worldmap );
    }
    else {
        painter->setPen( QPen( Qt::DashLine ) );
        painter->drawRect( QRectF( QPoint( 0, 0 ), mapRect.size().toSize() ) );

        for ( int y = 1; y < 4; ++y ) {
            if ( y == 2 ) {
                painter->setPen( QPen( Qt::DashLine ) );
            }
            else {
                painter->setPen( QPen( Qt::DotLine ) );
            }

            painter->drawLine( 0.0, 0.25 * y * mapRect.height(),
                                mapRect.width(), 0.25 * y * mapRect.height() );
        }
        for ( int x = 1; x < 8; ++x ) {
            if ( x == 4 ) {
                painter->setPen( QPen( Qt::DashLine ) );
            }
            else {
                painter->setPen( QPen( Qt::DotLine ) );
            }

            painter->drawLine( 0.125 * x * mapRect.width(), 0,
                               0.125 * x * mapRect.width(), mapRect.height() );
        }
    }

    m_target = target;

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
}

bool OverviewMap::eventFilter( QObject *object, QEvent *e )
{
    if ( !enabled() || !visible() ) {
        return false;
    }

    MarbleWidget *widget = dynamic_cast<MarbleWidget*>(object);
    if ( !widget ) {
        return AbstractFloatItem::eventFilter(object,e);
    }

    bool cursorAboveFloatItem(false);
    if ( e->type() == QEvent::MouseButtonDblClick || e->type() == QEvent::MouseMove ) {
        QMouseEvent *event = static_cast<QMouseEvent*>(e);
        QRectF floatItemRect = QRectF( positivePosition(), size() );

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

    return AbstractFloatItem::eventFilter(object,e);
}

void OverviewMap::changeBackground( const QString& target )
{
    delete m_svgobj;
    m_svgobj = 0;

    if ( target == "moon" ) {
        m_svgobj = new QSvgRenderer( MarbleDirs::path( "svg/lunarmap.svg" ),
                                    this );        
        return;
    }

    if ( target == "earth" ) {
        m_svgobj = new QSvgRenderer( MarbleDirs::path( "svg/worldmap.svg" ),
                                    this );
    }
}

}

Q_EXPORT_PLUGIN2( OverviewMap, Marble::OverviewMap )

#include "OverviewMap.moc"
