//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009      Andrew Manson <g.real.ate@gmail.com>
//

#ifndef OSMWAYGRAPHICSITEM_H
#define OSMWAYGRAPHICSITEM_H

#include "GeoGraphicsItem.h"

namespace Marble
{

class GeoDataPlacemark;
class GeoDataPoint;

class OsmWayGraphicsItem : public GeoGraphicsItem
{
public:
    OsmWayGraphicsItem();
    virtual ~OsmWayGraphicsItem();

    /**
     * This method is intended to be used only while parsing OSM files. It allows
     * for the reference ID of a node to be added to the way and for the actual
     * reference to the node to be added to the way later. This will be done on
     * a second pass of the data structure
     * @param reference the ID of the node to be filled in afterwards.
     */
    void addNodeReferenceId( int reference );

    /**
     * This method is intended to append a @see GeoDataPoint to the internal
     * data representation of this OSM Way item. This only appends to the end of
     * the Line String that represents this OSM Way and hense extends its length
     */
    void append( const GeoDataPoint& point );

    virtual void paint( GeoPainter* painter, ViewportParams *viewport,
                        const QString &renderPos, GeoSceneLayer *layer );

private:
    GeoDataPlacemark*   m_placemark;
};

}

#endif // OSMWAYGRAPHICSITEM_H
