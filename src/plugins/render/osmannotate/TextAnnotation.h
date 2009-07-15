//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009      Andrew Manson <g.real.ate@gmail.com>
//

#ifndef TEXTANNOTATION_H
#define TEXTANNOTATION_H

#include "TmpGraphicsItem.h"

namespace Marble{

class GeoDataPlacemark;

class TextAnnotation
{
public:
    TextAnnotation();

    virtual QString name() = 0 ;
    virtual QString description() = 0;
    virtual GeoDataGeometry geometry() = 0;

    GeoDataPlacemark toGeoData();

};

}

#endif // TEXTANNOTATION_H
