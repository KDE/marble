//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009      Bastian Holst <bastianholst@gmx.de>
//

// Self
#include "ScreenGraphicsItem.h"
#include "ScreenGraphicsItem_p.h"

using namespace Marble;

ScreenGraphicsItem::ScreenGraphicsItem()
    : MarbleGraphicsItem( new ScreenGraphicsItemPrivate )
{
}

ScreenGraphicsItem::ScreenGraphicsItem( ScreenGraphicsItemPrivate *d_ptr ) 
    : MarbleGraphicsItem( d_ptr )
{
}

ScreenGraphicsItem::~ScreenGraphicsItem() {
}

QPoint ScreenGraphicsItem::position() const {
    return p()->m_position;
}

void ScreenGraphicsItem::setPosition( const QPoint& position ) {
    p()->m_position = position;
}

ScreenGraphicsItemPrivate *ScreenGraphicsItem::p() const {
    return reinterpret_cast<ScreenGraphicsItemPrivate *>( d );
}
