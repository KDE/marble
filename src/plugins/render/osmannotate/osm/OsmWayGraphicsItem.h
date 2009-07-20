//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009      Andrew Manson <g.real.ate@gmail.com>
//

#ifndef OSMWAYGRAPHICSITEM_H
#define OSMWAYGRAPHICSITEM_H

#include "GeoLineStringGraphicsItem.h"
#include "marble_export.h"

namespace Marble{

class OsmWayGraphicsItem : public GeoLineStringGraphicsItem
{
public:
    OsmWayGraphicsItem();

    /**
     * This method is intended to be used only while parsing OSM files. It allows
     * for the reference ID of a node to be added to the way and for the actual
     * reference to the node to be added to the way later. This will be done on
     * a second pass of the data structure
     * @param reference the ID of the node to be filled in afterwards.
     */
    void addNodeReferenceId( int reference );

private:
    QList<int> m_nodeIdList;
};

}

#endif // OSMWAYGRAPHICSITEM_H
