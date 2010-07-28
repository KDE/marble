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

#include "MarbleDebug.h"


namespace Marble
{

TextAnnotation::TextAnnotation()
{
}

TextAnnotation::~TextAnnotation()
{
}

GeoDataPlacemark* TextAnnotation::toGeoData() const
{
    GeoDataPlacemark* placemark = new GeoDataPlacemark;

    placemark->setName( name() );
    placemark->setDescription( description() );
    //allow for HTML in the description
    placemark->setDescriptionCDATA( true );

    //FIXME: make this work for all geometries and not just points
//    FIXME change the geometry() to return a GeoDataGeometry and this won't work
    placemark->setGeometry( new GeoDataPoint( geometry() ) );

    return placemark;
}

}
