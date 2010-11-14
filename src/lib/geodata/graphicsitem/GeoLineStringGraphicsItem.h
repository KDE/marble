//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009      Andrew Manson <g.real.ate@gmail.com>
//

#ifndef MARBLE_GEOLINESTRINGGRAPHICSITEM_H
#define MARBLE_GEOLINESTRINGGRAPHICSITEM_H

#include "GeoDataLineString.h"
#include "GeoGraphicsItem.h"
#include "marble_export.h"

namespace Marble
{

class MARBLE_EXPORT GeoLineStringGraphicsItem : public GeoGraphicsItem
{
public:
    GeoLineStringGraphicsItem();

    void setLineString( const GeoDataLineString& lineString );
    void append( const GeoDataCoordinates& coordinates );

    virtual void paint( GeoPainter* painter, ViewportParams *viewport,
                        const QString &renderPos, GeoSceneLayer *layer );

protected:
    GeoDataLineString m_lineString;
};

}

#endif
