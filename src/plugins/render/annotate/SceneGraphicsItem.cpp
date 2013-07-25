//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009      Andrew Manson <g.real.ate@gmail.com>
// Copyright 2013      Thibaut Gridel <tgridel@free.fr>
//

#include "SceneGraphicsItem.h"

#include "GeoDataPlacemark.h"

#include <QVariant>

namespace Marble
{

SceneGraphicsItem::SceneGraphicsItem( GeoDataPlacemark *placemark )
    : GeoGraphicsItem( placemark ),
      m_placemark( placemark )
{
}

SceneGraphicsItem::~SceneGraphicsItem()
{
}

QList<QRegion> SceneGraphicsItem::regions() const
{
    return m_regions;
}

GeoDataPlacemark *SceneGraphicsItem::placemark()
{
    return m_placemark;
}

bool SceneGraphicsItem::sceneEvent( QEvent* event )
{
    if( event->type() == QEvent::MouseButtonPress ) {
        return mousePressEvent( static_cast<QMouseEvent*>( event ));
    }
    if( event->type() == QEvent::MouseMove ) {
        return mouseMoveEvent( static_cast<QMouseEvent*>( event ));
    }
    if( event->type() == QEvent::MouseButtonRelease ) {
        return mouseReleaseEvent( static_cast<QMouseEvent*>( event ));
    }

    return false;
}

bool SceneGraphicsItem::mousePressEvent( QMouseEvent* event )
{
    Q_UNUSED( event )
    //FIXME re-implement the whole ItemIsSelectable and call an
    //Item Change
    return false;
}

bool SceneGraphicsItem::mouseMoveEvent( QMouseEvent *event )
{
    Q_UNUSED( event )
    return false;
}

bool SceneGraphicsItem::mouseReleaseEvent( QMouseEvent *event )
{
    Q_UNUSED( event )
    return false;
}

void SceneGraphicsItem::setRegions( const QList<QRegion>& regions )
{
    m_regions = regions;
}

}

