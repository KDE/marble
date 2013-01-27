//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009      Andrew Manson <g.real.ate@gmail.com>
// Copyright 2013      Thibaut Gridel <tgridel@free.fr>
//
#include "AreaAnnotation.h"

#include "GeoDataPlacemark.h"
#include "GeoDataTypes.h"
#include "GeoPainter.h"
#include "ViewportParams.h"


namespace Marble
{



AreaAnnotation::AreaAnnotation( GeoDataPlacemark *placemark )
    :SceneGraphicsItem( placemark ),
      m_placemark( placemark ),
      m_movedPoint( -1 ),
      m_viewport( 0 )
{
}

void AreaAnnotation::paint(GeoPainter *painter, const ViewportParams *viewport )
{
    m_viewport = viewport;
    QList<QRegion> regionList;

    painter->save();
    painter->setBrush( QColor( 100, 100, 100 ) );
    if( m_placemark->geometry()->nodeType() == GeoDataTypes::GeoDataPolygonType ) {
        GeoDataPolygon *polygon = static_cast<GeoDataPolygon*>( m_placemark->geometry() );
        GeoDataLinearRing &ring = polygon->outerBoundary();
        for( int i=0; i< ring.size(); ++i ) {
            painter->drawEllipse( ring.at(i) , 10, 10 );
            regionList.append( painter->regionFromEllipse( ring.at(i), 10, 10 ));
        }
        regionList.append( painter->regionFromPolygon( ring, Qt::OddEvenFill ) );
    }
    painter->restore();
    setRegions( regionList );
}

bool AreaAnnotation::mousePressEvent( QMouseEvent *event )
{
    QList<QRegion> regionList = regions();
    for( int i=0; i< regionList.size()-1; ++i ) {
        if( regionList.at(i).contains( event->pos()) ) {
            m_movedPoint = i;
            return true;
        }
    }
    return false;
}

bool AreaAnnotation::mouseMoveEvent( QMouseEvent *event )
{
    if( !m_viewport
        || m_movedPoint < 0 ) {
        return false;
    }
    GeoDataPolygon *polygon = static_cast<GeoDataPolygon*>( m_placemark->geometry() );
    GeoDataLinearRing &ring = polygon->outerBoundary();
    qreal lon, lat;
    m_viewport->geoCoordinates( event->pos().x(),
                                event->pos().y(),
                                lon, lat,
                                GeoDataCoordinates::Radian );
    ring[m_movedPoint] = GeoDataCoordinates( lon, lat );
    return true;
}

bool AreaAnnotation::mouseReleaseEvent( QMouseEvent *event )
{
    m_movedPoint = -1;
    return true;
}

}
