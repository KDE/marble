/*
    Copyright (C) 2008 Jens-Michael Hoffmann <jensmh@gmx.de>

    This file is part of the KDE project

    This library is free software you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    aint with this library see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/

#ifndef OsmNamefinder_ElementDictionary_h
#define OsmNamefinder_ElementDictionary_h

#include <QtCore/QString>

#include <marble_export.h>

namespace Marble
{
namespace OsmNamefinder
{

extern const char *tag_namespace;
extern const char *tag_searchresults;
extern const char *tag_named;
extern const char *tag_distance;
extern const char *tag_description;
extern const char *tag_place;
extern const char *tag_nearestplaces;

enum ItemType { UndefinedType, Node, Way, Segment };

ItemType getItemType( const QString & );

}
}

#endif
