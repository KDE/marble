//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2013   Utku Aydın <utkuaydin34@gmail.com>
//

#include "MergeItem.h"

#include "GeoDataPlacemark.h"

namespace Marble {

MergeItem::MergeItem()
{
}

QString MergeItem::pathA()
{
    return m_pathA;
}

void MergeItem::setPathA( const QString &path )
{
    m_pathA = path;
    pathAChanged();
}

QString MergeItem::pathB()
{
    return m_pathB;
}

void MergeItem::setPathB( const QString &path )
{
    m_pathB = path;
    pathBChanged();
}

GeoDataPlacemark MergeItem::placemarkA()
{
    return m_placemarkA;
}

void MergeItem::setPlacemarkA( const GeoDataPlacemark &placemark )
{
    m_placemarkA = placemark;
    emit placemarkAChanged();
    emit nameAChanged();
    emit descriptionAChanged();
}

GeoDataPlacemark MergeItem::placemarkB()
{
    return m_placemarkB;
}

void MergeItem::setPlacemarkB( const GeoDataPlacemark &placemark )
{
    m_placemarkB = placemark;
    emit placemarkBChanged();
    emit nameBChanged();
    emit descriptionBChanged();
}

QString MergeItem::nameA()
{
    return m_placemarkA.name();
}

QString MergeItem::nameB()
{
    return m_placemarkB.name();
}

QString MergeItem::descriptionA()
{
    return m_placemarkA.description();
}

QString MergeItem::descriptionB()
{
    return m_placemarkB.description();
}

MergeItem::Action MergeItem::actionA()
{
    return m_actionA;
}

void MergeItem::setActionA( MergeItem::Action action )
{
    m_actionA = action;
}

MergeItem::Action MergeItem::actionB()
{
    return m_actionB;
}

void MergeItem::setActionB( MergeItem::Action action )
{
    m_actionB = action;
}

MergeItem::Resolution MergeItem::resolution()
{
    return m_resolution;
}

void MergeItem::setResolution( MergeItem::Resolution resolution )
{
    m_resolution = resolution;
    emit resolutionChanged();
}

}

#include "MergeItem.moc"
