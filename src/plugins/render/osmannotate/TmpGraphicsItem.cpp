//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009      Andrew Manson <g.real.ate@gmail.com>
//

#include "TmpGraphicsItem.h"

#include <QtCore/QVariant>

namespace Marble
{

TmpGraphicsItem::TmpGraphicsItem()
{
    m_parent = 0;

    //sensible default?
    geoOffset = false;
}

TmpGraphicsItem::~TmpGraphicsItem()
{

}

QVariant TmpGraphicsItem::itemChange(GeoGraphicsItemChange change, QVariant v )
{
    Q_UNUSED(change);
    Q_UNUSED(v);
    return QVariant();
}

QList<QRegion> TmpGraphicsItem::regions() const
{
    return m_regions;
}

QList<TmpGraphicsItem*> TmpGraphicsItem::children() const
{
    return m_children;
}

void TmpGraphicsItem::addChild(TmpGraphicsItem* c)
{
    m_children.append(c);
}

TmpGraphicsItem* TmpGraphicsItem::parent()
{
    return m_parent;
}

void TmpGraphicsItem::setParent( TmpGraphicsItem* p )
{
    m_parent = p;
}

bool TmpGraphicsItem::sceneEvent( QEvent* event )
{
    if( event->type() == QEvent::MouseButtonPress ) {
        return mousePressEvent( static_cast<QMouseEvent*>( event ));
    }

    return false;
}

bool TmpGraphicsItem::mousePressEvent( QMouseEvent* event )
{
    Q_UNUSED( event )
    //FIXME re-implement the whole ItemIsSelectable and call an
    //Item Change
    return false;
}

void TmpGraphicsItem::setGeoOffset( bool g )
{
    geoOffset = g;
}

void TmpGraphicsItem::setRegions( const QList<QRegion>& regions )
{
    m_regions.clear();
    m_regions = regions;
}

}

