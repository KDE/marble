//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009      Andrew Manson <g.real.ate@gmail.com>
//

#ifndef OSMBOUNDSGRAPHICSITEM_H
#define OSMBOUNDSGRAPHICSITEM_H

#include "GeoDataLineString.h"
#include "GeoDataGeometry.h"
#include "GeoGraphicsItem.h"

#include <QtGui/QPen>

namespace Marble
{

class GeoPainter;
class ViewportParams;
class GeoSceneLayer;

class OsmBoundsGraphicsItem : public GeoGraphicsItem
{
public:
    OsmBoundsGraphicsItem();
    explicit OsmBoundsGraphicsItem( const GeoDataLineString& other );

    void append ( const GeoDataCoordinates& value );

    virtual void paint( GeoPainter* painter, ViewportParams *viewport,
                        const QString &renderPos, GeoSceneLayer *layer );
    virtual GeoDataLatLonBox    latLonBox();
private:
    /**
     * This class is using a GeoDataLineString as its data representation so
     * that it can be closer to the visual representation. It does not make
     * sense to use a LatLon box as it does not explicitly have a geographical
     * representation.
     */
    GeoDataLineString   m_lineString;

    QPen    m_pen;
};

}

#endif // OSMBOUNDSGRAPHICSITEM_H
