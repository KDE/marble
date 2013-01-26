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

#include "SceneGraphicsItem.h"
#include "GeoDataPolygon.h"

namespace Marble
{

class  AreaAnnotation : public SceneGraphicsItem
{
public:
    AreaAnnotation( GeoDataPlacemark *placemark );

    virtual void paint( GeoPainter *painter, const ViewportParams *viewport );
    virtual bool mousePressEvent( QMouseEvent *event );
    virtual bool mouseMoveEvent( QMouseEvent *event );
    virtual bool mouseReleaseEvent( QMouseEvent *event );

private:
    GeoDataPlacemark *m_placemark;
    int m_movedPoint;
    const ViewportParams *m_viewport;
};

}

#endif // AREAANNOTATION_H
