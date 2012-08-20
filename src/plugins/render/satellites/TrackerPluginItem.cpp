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

#include "MarbleDebug.h"

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
    delete d;
}

GeoDataPlacemark* TrackerPluginItem::placemark()
{
    return d->m_placemark;
}

} // namespace Marble

