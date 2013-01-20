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
#include "GeoPainter.h"


namespace Marble
{



AreaAnnotation::AreaAnnotation( GeoDataPlacemark *placemark )
    :TmpGraphicsItem( placemark )
{
}

void AreaAnnotation::paint(GeoPainter *painter, const ViewportParams *viewport )
{
    Q_UNUSED(viewport);
    painter->save();

    painter->setBrush( QBrush( QColor( 0, 255, 255, 80 )  ) );
    const GeoDataPlacemark *placemark = static_cast<const GeoDataPlacemark*>( feature() );
    GeoDataPolygon *polygon = static_cast<GeoDataPolygon*>( placemark->geometry() );
    painter->drawPolygon( *polygon );

    painter->restore();

}

}
