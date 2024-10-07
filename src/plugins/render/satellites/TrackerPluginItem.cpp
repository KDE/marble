// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2011 Guillaume Martres <smarter@ubuntu.com>
//

#include "TrackerPluginItem.h"
#include "GeoDataPlacemark.h"

namespace Marble
{

class TrackerPluginItemPrivate
{
public:
    TrackerPluginItemPrivate(const QString &name, GeoDataPlacemark *placemark)
        : m_name(name)
        , m_placemark(placemark)
    {
    }

    const QString m_name;
    GeoDataPlacemark *const m_placemark;
    bool m_enabled = false;
    bool m_trackVisible = false;
};

TrackerPluginItem::TrackerPluginItem(const QString &name)
    : d(new TrackerPluginItemPrivate(name, new GeoDataPlacemark(name)))
{
}

TrackerPluginItem::~TrackerPluginItem()
{
    delete d;
}

QString TrackerPluginItem::name() const
{
    return d->m_name;
}

GeoDataPlacemark *TrackerPluginItem::placemark()
{
    return d->m_placemark;
}

bool TrackerPluginItem::isEnabled() const
{
    return d->m_enabled;
}

void TrackerPluginItem::setEnabled(bool enabled)
{
    d->m_enabled = enabled;
}

bool TrackerPluginItem::isVisible() const
{
    return d->m_placemark->isVisible();
}

void TrackerPluginItem::setVisible(bool visible)
{
    d->m_placemark->setVisible(visible);
}

bool TrackerPluginItem::isTrackVisible() const
{
    return d->m_trackVisible;
}

void TrackerPluginItem::setTrackVisible(bool visible)
{
    d->m_trackVisible = visible;
}

} // namespace Marble
