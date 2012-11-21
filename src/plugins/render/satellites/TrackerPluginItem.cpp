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
        : m_placemark( placemark ),
          m_enabled( false )
    {
    }

    GeoDataPlacemark *m_placemark;
    bool m_enabled;
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

bool TrackerPluginItem::isEnabled() const
{
    return d->m_enabled;
}

void TrackerPluginItem::setEnabled( bool enabled )
{
    d->m_enabled = enabled;
}

bool TrackerPluginItem::isVisible() const
{
    return d->m_placemark->isVisible();
}

void TrackerPluginItem::setVisible( bool visible )
{
    d->m_placemark->setVisible( visible );
}

} // namespace Marble

#include "TrackerPluginItem.moc"

