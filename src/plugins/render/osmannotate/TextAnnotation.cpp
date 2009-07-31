//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009      Andrew Manson <g.real.ate@gmail.com>
//

#include "TextAnnotation.h"

#include "GeoDataPlacemark.h"


namespace Marble{

TextAnnotation::TextAnnotation()
{
}

TextAnnotation::~TextAnnotation()
{
}

GeoDataPlacemark TextAnnotation::toGeoData()
{
    GeoDataPlacemark placemark;

    placemark.setName( name() );
    placemark.setDescription( description() );

    //FIXME: make this work for all geometries and not just points
//    placemark.setGeometry( geometry() );
    placemark.setGeometry( GeoDataPoint( geometry() ) );

    return placemark;
}

}
