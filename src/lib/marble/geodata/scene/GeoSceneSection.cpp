/*
    SPDX-FileCopyrightText: 2008 Torsten Rahn <rahn@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "GeoSceneSection.h"

#include "GeoSceneItem.h"
#include "GeoSceneTypes.h"

namespace Marble
{

GeoSceneSection::GeoSceneSection(const QString &name)
    : m_name(name)
    , m_checkable(false)
    , m_spacing(12)
{
}

GeoSceneSection::~GeoSceneSection()
{
    qDeleteAll(m_items);
}

const char *GeoSceneSection::nodeType() const
{
    return GeoSceneTypes::GeoSceneSectionType;
}

void GeoSceneSection::addItem(GeoSceneItem *item)
{
    // Remove any item that has the same name
    QList<GeoSceneItem *>::iterator it = m_items.begin();
    while (it != m_items.end()) {
        GeoSceneItem *currentItem = *it;
        if (currentItem->name() == item->name()) {
            delete currentItem;
            m_items.erase(it);
            break;
        } else {
            ++it;
        }
    }

    if (item) {
        m_items.append(item);
    }
}

GeoSceneItem *GeoSceneSection::item(const QString &name)
{
    GeoSceneItem *item = nullptr;

    QList<GeoSceneItem *>::const_iterator it = m_items.constBegin();
    QList<GeoSceneItem *>::const_iterator end = m_items.constEnd();
    for (; it != end; ++it) {
        if ((*it)->name() == name) {
            item = *it;
            break;
        }
    }

    if (!item) {
        item = new GeoSceneItem(name);
        addItem(item);
    }

    return item;
}

QList<GeoSceneItem *> GeoSceneSection::items() const
{
    return m_items;
}

QString GeoSceneSection::name() const
{
    return m_name;
}

QString GeoSceneSection::heading() const
{
    return m_heading;
}

void GeoSceneSection::setHeading(const QString &heading)
{
    m_heading = heading;
}

bool GeoSceneSection::checkable() const
{
    return m_checkable;
}

void GeoSceneSection::setCheckable(bool checkable)
{
    m_checkable = checkable;
}

QString GeoSceneSection::connectTo() const
{
    return m_connectTo;
}

void GeoSceneSection::setConnectTo(const QString &connectTo)
{
    m_connectTo = connectTo;
}

int GeoSceneSection::spacing() const
{
    return m_spacing;
}

void GeoSceneSection::setSpacing(int spacing)
{
    m_spacing = spacing;
}

QString GeoSceneSection::radio() const
{
    return m_radio;
}

void GeoSceneSection::setRadio(const QString &radio)
{
    m_radio = radio;
}

}
