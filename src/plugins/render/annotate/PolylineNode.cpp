// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2014 Calin Cruceru <crucerucalincristian@gmail.com>
//

#include "PolylineNode.h"

namespace Marble
{

PolylineNode::PolylineNode( const QRegion& region ) :
    m_region( region  ),
    m_flags( nullptr )
{
    // nothing to do
}

PolylineNode::~PolylineNode()
{
    // nothing to do
}

bool PolylineNode::isSelected() const
{
    return m_flags & NodeIsSelected;
}

bool PolylineNode::isBeingMerged() const
{
    return m_flags & NodeIsMerged;
}

bool PolylineNode::isEditingHighlighted() const
{
    return m_flags & NodeIsEditingHighlighted;
}

bool PolylineNode::isMergingHighlighted() const
{
    return m_flags & NodeIsMergingHighlighted;
}

void PolylineNode::setRegion( const QRegion& newRegion )
{
    m_region = newRegion;
}

PolylineNode::PolyNodeFlags PolylineNode::flags() const
{
    return m_flags;
}

void PolylineNode::setFlag( PolyNodeFlag flag, bool enabled )
{
    if ( enabled ) {
        m_flags |= flag;
    } else {
        m_flags &= ~flag;
    }
}

void PolylineNode::setFlags( PolyNodeFlags flags )
{
    m_flags = flags;
}

bool PolylineNode::containsPoint( const QPoint &eventPos ) const
{
    return m_region.contains( eventPos );
}

}
