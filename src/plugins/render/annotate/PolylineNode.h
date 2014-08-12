//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2014      Calin Cruceru  <crucerucalincristian@gmail.com>
//

#ifndef POLYLINENODE_H
#define POLYLINENODE_H

#include <QRegion>
#include <QPoint>

namespace Marble
{

class PolylineNode
{
public:
    enum PolyNodeFlag {
        NoOption = 0x0,
        NodeIsSelected = 0x1,
        NodeIsInnerTmp = 0x2,
        NodeIsMerged = 0x4,
        NodeIsEditingHighlighted = 0x8,
        NodeIsMergingHighlighted = 0x10
    };
    Q_DECLARE_FLAGS(PolyNodeFlags, PolyNodeFlag)

    explicit PolylineNode( QRegion region, PolyNodeFlags flags = 0 );
    ~PolylineNode();

    bool isSelected() const;
    bool isInnerTmp() const;
    bool isBeingMerged() const;
    bool isEditingHighlighted() const;
    bool isMergingHighlighted() const;

    PolyNodeFlags flags() const;

    void setFlag( PolyNodeFlag flag, bool enabled = true );
    void setFlags( PolyNodeFlags flags );
    void setRegion( QRegion newRegion );

    bool containsPoint( const QPoint &eventPos ) const;

private:
    QRegion m_region;
    PolyNodeFlags m_flags;
};

}

#endif
