// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2013 Utku Aydın <utkuaydin34@gmail.com>
//

#include "MergeItem.h"

namespace Marble
{

MergeItem::MergeItem() = default;

QString MergeItem::pathA() const
{
    return m_pathA;
}

void MergeItem::setPathA(const QString &path)
{
    m_pathA = path;
    Q_EMIT pathAChanged();
}

QString MergeItem::pathB() const
{
    return m_pathB;
}

void MergeItem::setPathB(const QString &path)
{
    m_pathB = path;
    Q_EMIT pathBChanged();
}

GeoDataPlacemark MergeItem::placemarkA() const
{
    return m_placemarkA;
}

void MergeItem::setPlacemarkA(const GeoDataPlacemark &placemark)
{
    m_placemarkA = placemark;
    Q_EMIT placemarkAChanged();
    Q_EMIT nameAChanged();
    Q_EMIT descriptionAChanged();
}

GeoDataPlacemark MergeItem::placemarkB() const
{
    return m_placemarkB;
}

void MergeItem::setPlacemarkB(const GeoDataPlacemark &placemark)
{
    m_placemarkB = placemark;
    Q_EMIT placemarkBChanged();
    Q_EMIT nameBChanged();
    Q_EMIT descriptionBChanged();
}

QString MergeItem::nameA() const
{
    return m_placemarkA.name();
}

QString MergeItem::nameB() const
{
    return m_placemarkB.name();
}

QString MergeItem::descriptionA() const
{
    return m_placemarkA.description();
}

QString MergeItem::descriptionB() const
{
    return m_placemarkB.description();
}

MergeItem::Action MergeItem::actionA() const
{
    return m_actionA;
}

void MergeItem::setActionA(MergeItem::Action action)
{
    m_actionA = action;
}

MergeItem::Action MergeItem::actionB() const
{
    return m_actionB;
}

void MergeItem::setActionB(MergeItem::Action action)
{
    m_actionB = action;
}

MergeItem::Resolution MergeItem::resolution() const
{
    return m_resolution;
}

void MergeItem::setResolution(MergeItem::Resolution resolution)
{
    m_resolution = resolution;
    Q_EMIT resolutionChanged();
}

}

#include "moc_MergeItem.cpp"
