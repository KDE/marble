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

#include "osm-namefinder/ElementDictionary.h"

namespace Marble
{
namespace OsmNamefinder
{

// One list of all element names
const char *tag_namespace     = "";
const char *tag_searchresults = "searchresults";
const char *tag_named         = "named";
const char *tag_distance      = "distance";
const char *tag_description   = "description";
const char *tag_place         = "place";
const char *tag_nearestplaces = "nearestplaces";

ItemType getItemType( const QString & s )
{
    ItemType result = UndefinedType;
    if ( s == "Node" )
        result = Node;
    else if ( s == "Way" )
        result = Way;
    else if ( s == "Segment" )
        result = Segment;
    return result;
}

}
}
