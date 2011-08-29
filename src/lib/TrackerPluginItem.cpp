//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011 Guillaume Martres <smarter@ubuntu.com>
//

#include "TrackerPluginItem.h"

#include "GeoDataPlacemark.h"

namespace Marble {

class TrackerPluginItemPrivate
{
public:
    TrackerPluginItemPrivate( GeoDataPlacemark *placemark )
        : m_placemark( placemark )
    {
    }

    GeoDataPlacemark *m_placemark;
};

TrackerPluginItem::TrackerPluginItem( const QString &name )
    : d( new TrackerPluginItemPrivate( new GeoDataPlacemark( name ) ) )
{
}

TrackerPluginItem::~TrackerPluginItem()
{
}

GeoDataPlacemark* TrackerPluginItem::placemark()
{
    return d->m_placemark;
}

void TrackerPluginItem::render( GeoPainter *painter, ViewportParams *viewport, const QString &renderPos, GeoSceneLayer *layer )
{
    Q_UNUSED( painter );
    Q_UNUSED( viewport );
    Q_UNUSED( renderPos );
    Q_UNUSED( layer );
}

}
