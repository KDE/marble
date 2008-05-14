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

    painter->setViewport( contentRect().toRect() );
    m_svgobj->render( painter ); 
    painter->setViewport( QRect( QPoint( 0, 0 ), viewport->size() ) );

/*
    GeoDataPoint northpole1( 0.0, -90.0, 0.0, GeoDataPoint::Degree );
    GeoDataPoint northpole2( 0.0, -90.0, 3000000.0, GeoDataPoint::Degree );

    painter->setPen( QColor( 255, 255, 255, 255 ) );

    painter->drawLine( northpole1, northpole2 );
*/
    return true;
}

Q_EXPORT_PLUGIN2(MarbleOverviewMap, MarbleOverviewMap)

#include "MarbleOverviewMap.moc"
