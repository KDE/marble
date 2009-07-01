//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009      Andrew Manson <g.real.ate@gmail.com>
//

#include "TmpGraphicsItem.h"

#include <QtCore/QVariant>

namespace Marble{

TmpGraphicsItem::TmpGraphicsItem()
{
    parent = 0;

    //sensible default?
    geoOffset = false;
}

TmpGraphicsItem::~TmpGraphicsItem()
{

}

QVariant TmpGraphicsItem::itemChange(GeoGraphicsItemChange change, QVariant v )
{
    return QVariant();
}

QList<TmpGraphicsItem*> TmpGraphicsItem::getChildren()
{
    return QList<TmpGraphicsItem*>(children);
}

void TmpGraphicsItem::addChild(TmpGraphicsItem* c)
{
    children.append(c);
}

TmpGraphicsItem* TmpGraphicsItem::getParent()
{
    return parent;
}

void TmpGraphicsItem::setParent( TmpGraphicsItem* p )
{
    parent = p;
}

void TmpGraphicsItem::setGeoOffset( bool g )
{
    geoOffset = g;
}

}

