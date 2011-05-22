//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011      Dennis Nienhüser <earthwings@gentoo.org>
//

#ifndef OSMREGIONTREE_H
#define OSMREGIONTREE_H

#include "OsmRegion.h"

#include <QtCore/QVector>

namespace Marble
{

class OsmRegionTree
{
public:
    OsmRegionTree( const OsmRegion & node = OsmRegion() );

    const OsmRegion &node() const;

    void setChildren( const QVector<OsmRegionTree> children );

    const QVector<OsmRegionTree> & children() const;

    void append( QList<OsmRegion> &regions );

    void traverse( int &counter );

    operator QList<OsmRegion>() const;

    int smallestRegionId( const GeoDataCoordinates &coordinates ) const;

private:
    int smallestRegionId( const GeoDataCoordinates &coordinates, int &level ) const;

    void enumerate( QList<OsmRegion> &list ) const;

    OsmRegion m_node;

    QVector<OsmRegionTree> m_children;
};

}

#endif // OSMREGIONTREE_H

