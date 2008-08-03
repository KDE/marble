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
#include "osm-namefinder/DistanceTagHandler.h"

#include <QtCore/QDebug>

#include "GeoParser.h"
#include "osm-namefinder/AttributeDictionary.h"
#include "osm-namefinder/Distance.h"
#include "osm-namefinder/ElementDictionary.h"
#include "osm-namefinder/SearchResults.h"

namespace Marble
{
namespace OsmNamefinder
{

static GeoTagHandlerRegistrar
handler( GeoTagHandler::QualifiedName( tag_distance, tag_namespace ),
	 new DistanceTagHandler );


GeoNode * DistanceTagHandler::parse( GeoParser & parser ) const
{
    Q_ASSERT( parser.isStartElement() && parser.isValidElement( tag_distance ));
    qDebug() << "DistanceTagHandler";

    GeoStackItem parentItem = parser.parentElement();
    Q_ASSERT( parentItem.represents( tag_searchresults ));

    // attribute fromtype
    ItemType fromType = UndefinedType;
    const QString fromTypeStr = parser.attribute( attr_fromtype ).trimmed();
    fromType = getItemType( fromTypeStr );

    // attribute from
    int fromId = 0;
    const QString fromIdStr = parser.attribute( attr_from ).trimmed();
    if ( !fromIdStr.isEmpty() )
      fromId = fromIdStr.toInt();

    // attribute totype
    ItemType toType = UndefinedType;
    const QString toTypeStr = parser.attribute( attr_totype ).trimmed();
    toType = getItemType( toTypeStr );

    // attribute to
    int toId = 0;
    const QString toIdStr = parser.attribute( attr_to ).trimmed();
    if ( !toIdStr.isEmpty() )
      toId = toIdStr.toInt();

    // element text
    double distanceKm = 0.0;
    const QString distanceKmStr = parser.readElementText().trimmed();
    if ( !distanceKmStr.isEmpty() )
      distanceKm = distanceKmStr.toDouble();

    Distance *distance = new Distance( fromType, fromId, toType, toId, distanceKm );
    parentItem.nodeAs<OnfSearchResults>()->addDistance( distance );
    return 0;
}
