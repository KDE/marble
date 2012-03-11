//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011      Dennis Nienhüser <earthwings@gentoo.org>
//

#include "OsmRegionTree.h"

namespace Marble
{

OsmRegionTree::OsmRegionTree( const OsmRegion & node ) :
    m_node( node )
{
    // nothing to do
}

const OsmRegion &OsmRegionTree::node() const
{
    return m_node;
}

void OsmRegionTree::setChildren( const QVector<OsmRegionTree> children )
{
    m_children = children;
}

const QVector<OsmRegionTree> & OsmRegionTree::children() const
{
    return m_children;
}

void OsmRegionTree::append( QList<OsmRegion> &regions )
{
    foreach( const OsmRegion &candidate, regions ) {
        if ( candidate.parentIdentifier() == m_node.identifier() ) {
            m_children << OsmRegionTree( candidate );
        }
    }

    foreach( const OsmRegionTree & child, m_children ) {
        regions.removeAll( child.node() );
    }

    for ( int i = 0; i < m_children.size(); ++i ) {
        m_children[i].append( regions );
    }
}

void OsmRegionTree::traverse( int &counter )
{
    ++counter;
    m_node.setLeft( counter );

    for( int i = 0; i < m_children.size(); ++i ) {
        m_children[i].traverse( counter );
    }

    ++counter;
    m_node.setRight( counter );
}

OsmRegionTree::operator QList<OsmRegion>() const
{
    QList<OsmRegion> result;
    enumerate( result );
    return result;
}

void OsmRegionTree::enumerate( QList<OsmRegion> &list ) const
{
    list << m_node;
    foreach( const OsmRegionTree & child, m_children ) {
        child.enumerate( list );
    }
}

int OsmRegionTree::smallestRegionId( const GeoDataCoordinates &coordinates ) const
{
    int rootLevel = m_node.adminLevel();
    return smallestRegionId( coordinates, rootLevel );
}

int OsmRegionTree::smallestRegionId( const GeoDataCoordinates &coordinates, int &level ) const
{
    int maxLevel = m_node.adminLevel();
    int minId = m_node.identifier();
    foreach( const OsmRegionTree & child, m_children ) {
        if ( child.node().geometry().contains( coordinates ) ) {
            int childLevel = level;
            int id = child.smallestRegionId( coordinates, childLevel );
            if ( childLevel >= maxLevel ) {
                maxLevel = childLevel;
                minId = id;
            }
        }
    }

    level = maxLevel;
    return minId;
}

}
