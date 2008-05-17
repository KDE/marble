//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2008 Torsten Rahn <tackat@kde.org>"
//

#include "CompassFloatItem.h"

#include <QtCore/QRect>
#include <QtGui/QColor>
#include <QtGui/QPixmap>
#include <QtSvg/QSvgRenderer>

#include "MarbleDirs.h"
#include "GeoPainter.h"
#include "GeoDataPoint.h"
#include "ViewportParams.h"


MarbleCompassFloatItem::MarbleCompassFloatItem( const QPointF &point, const QSizeF &size )
    : MarbleAbstractFloatItem( point, size )
{
    m_svgobj = new QSvgRenderer( MarbleDirs::path( "svg/compass.svg" ),
                                 this );
}

MarbleCompassFloatItem::~MarbleCompassFloatItem()
{
    delete m_svgobj;
}

QStringList MarbleCompassFloatItem::backendTypes() const
{
    return QStringList( "compass" );
}

QString MarbleCompassFloatItem::name() const
{
    return tr( "Compass" );
}

QString MarbleCompassFloatItem::guiString() const
{
    return tr( "&Compass" );
}

QString MarbleCompassFloatItem::nameId() const
{
    return QString( "compass" );
}

QString MarbleCompassFloatItem::description() const
{
    return tr( "This is a float item that provides a compass." );
}

QIcon MarbleCompassFloatItem::icon () const
{
    return QIcon();
}


void MarbleCompassFloatItem::initialize ()
{
}

bool MarbleCompassFloatItem::isInitialized () const
{
    return true;
}

QPainterPath MarbleCompassFloatItem::backgroundShape() const
{
    QPainterPath path;
    path.addEllipse( QRectF( QPointF( marginLeft(), marginTop() ), renderedRect().size() ).toRect() );
    return path;
}

bool MarbleCompassFloatItem::needsUpdate( ViewportParams *viewport )
{
// figure out the polarity ...

    return true;
}

bool MarbleCompassFloatItem::renderFloatItem( GeoPainter *painter, ViewportParams *viewport, GeoSceneLayer * layer )
{
    painter->save();
    painter->autoMapQuality();

    QRectF mapRect( contentRect() );

    // Rerender worldmap pixmap if the size has changed
    if ( m_compass.size() != mapRect.size().toSize() ) {
        m_compass = QPixmap( mapRect.size().toSize() );
        m_compass.fill( Qt::transparent );
        QPainter mapPainter( &m_compass );
        mapPainter.setViewport( m_compass.rect() );
        m_svgobj->render( &mapPainter ); 
        mapPainter.setViewport( QRect( QPoint( 0, 0 ), viewport->size() ) );
    }
    painter->drawPixmap( QPoint( 0, 0 ), m_compass );

    painter->restore();

    return true;
}

Q_EXPORT_PLUGIN2(MarbleCompassFloatItem, MarbleCompassFloatItem)

#include "CompassFloatItem.moc"
