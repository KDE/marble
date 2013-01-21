//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009      Andrew Manson <g.real.ate@gmail.com>
//
#include "AreaAnnotation.h"

#include "GeoDataPlacemark.h"
#include "GeoDataTypes.h"
#include "GeoPainter.h"


namespace Marble
{



AreaAnnotation::AreaAnnotation( GeoDataPlacemark *placemark )
    :TmpGraphicsItem( placemark ),
      m_placemark( placemark )
{
}

void AreaAnnotation::paint(GeoPainter *painter, const ViewportParams *viewport )
{
    Q_UNUSED(viewport);
    painter->save();
    painter->setBrush( QColor( 100, 100, 100 ) );
    if( m_placemark->geometry()->nodeType() == GeoDataTypes::GeoDataPolygonType ) {
        GeoDataPolygon *polygon = static_cast<GeoDataPolygon*>( m_placemark->geometry() );
        GeoDataLinearRing &ring = polygon->outerBoundary();
        for( int i=0; i< ring.size(); ++i ) {
            painter->drawEllipse( ring.at(i) , 10, 10 );
        }
    }
    painter->restore();
}

}
