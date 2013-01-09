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

#include "GeoGraphicsItem.h"
#include "marble_export.h"

namespace Marble
{

class GeoDataLineString;
class GeoDataLineStyle;

class MARBLE_EXPORT GeoLineStringGraphicsItem : public GeoGraphicsItem
{
public:
    explicit GeoLineStringGraphicsItem( const GeoDataLineString *lineString );

    void setLineString( const GeoDataLineString* lineString );

    virtual const GeoDataLatLonAltBox& latLonAltBox() const;

    virtual void paint( GeoPainter* painter, const ViewportParams *viewport );

protected:
    const GeoDataLineString *m_lineString;
};

}

#endif
