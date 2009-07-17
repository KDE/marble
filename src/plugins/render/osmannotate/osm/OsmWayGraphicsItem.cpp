//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009      Andrew Manson <g.real.ate@gmail.com>
//

#include "OsmWayGraphicsItem.h"

namespace Marble
{

OsmWayGraphicsItem::OsmWayGraphicsItem()
        : GeoLineStringGraphicsItem()
{
}

void OsmWayGraphicsItem::addNodeReferenceId( int reference )
{
    m_nodeIdList.append( reference );
}

}
