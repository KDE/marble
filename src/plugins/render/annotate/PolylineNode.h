// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2014 Calin Cruceru <crucerucalincristian@gmail.com>
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
        NodeIsMerged = 0x2,
        NodeIsEditingHighlighted = 0x4,
        NodeIsMergingHighlighted = 0x8
    };
    Q_DECLARE_FLAGS(PolyNodeFlags, PolyNodeFlag)

    explicit PolylineNode( const QRegion& region = QRegion() );
    ~PolylineNode();

    bool isSelected() const;
    bool isBeingMerged() const;
    bool isEditingHighlighted() const;
    bool isMergingHighlighted() const;

    PolyNodeFlags flags() const;

    void setFlag( PolyNodeFlag flag, bool enabled = true );
    void setFlags( PolyNodeFlags flags );
    void setRegion( const QRegion& newRegion );

    bool containsPoint( const QPoint &eventPos ) const;

private:
    QRegion m_region;
    PolyNodeFlags m_flags;
};

}

Q_DECLARE_TYPEINFO(Marble::PolylineNode, Q_MOVABLE_TYPE);

#endif
