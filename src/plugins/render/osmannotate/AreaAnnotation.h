//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009      Andrew Manson <g.real.ate@gmail.com>
//

#ifndef AREAANNOTATION_H
#define AREAANNOTATION_H

#include "TmpGraphicsItem.h"
#include "GeoDataPolygon.h"

namespace Marble
{

class  AreaAnnotation : public TmpGraphicsItem
{
public:
    AreaAnnotation( GeoDataFeature *feature );

    virtual void paint( GeoPainter *painter, const ViewportParams *viewport );
    //FIXME Waiting to be removed
    virtual QRect screenBounding(){return QRect();}
};

}

#endif // AREAANNOTATION_H
