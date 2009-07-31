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

#include <QtCore/QString>

#include "GeoDataPlacemark.h"

namespace Marble{

class GeoDataPlacemark;

class TextAnnotation
{
public:
    TextAnnotation();
    virtual ~TextAnnotation();

    virtual QString name() const = 0 ;
    virtual QString description() const = 0;
    virtual GeoDataGeometry geometry() const = 0;

    GeoDataPlacemark toGeoData() const;

};

}

#endif // TEXTANNOTATION_H
