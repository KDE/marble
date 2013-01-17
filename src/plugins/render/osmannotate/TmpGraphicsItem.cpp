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

TmpGraphicsItem::TmpGraphicsItem( GeoDataFeature *feature )
    : GeoGraphicsItem( feature )
{
}

TmpGraphicsItem::~TmpGraphicsItem()
{
}

QList<QRegion> TmpGraphicsItem::regions() const
{
    return m_regions;
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

void TmpGraphicsItem::setRegions( const QList<QRegion>& regions )
{
    m_regions.clear();
    m_regions = regions;
}

}

