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
    Q_UNUSED(painter);
    Q_UNUSED(viewport);
}

}
