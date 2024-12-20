// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2011 Dennis Nienhüser <nienhueser@kde.org>
//

#ifndef OSMREGIONTREE_H
#define OSMREGIONTREE_H

#include "OsmRegion.h"

#include <QList>

namespace Marble
{

class OsmRegionTree
{
public:
    explicit OsmRegionTree(const OsmRegion &node = OsmRegion());

    const OsmRegion &node() const;

    void setChildren(const QList<OsmRegionTree> &children);

    const QList<OsmRegionTree> &children() const;

    void append(QList<OsmRegion> &regions);

    void traverse(int &counter);

    operator QList<OsmRegion>() const;

    int smallestRegionId(const GeoDataCoordinates &coordinates) const;

private:
    int smallestRegionId(const GeoDataCoordinates &coordinates, int &level) const;

    void enumerate(QList<OsmRegion> &list) const;

    OsmRegion m_node;

    QList<OsmRegionTree> m_children;
};

}

#endif // OSMREGIONTREE_H
