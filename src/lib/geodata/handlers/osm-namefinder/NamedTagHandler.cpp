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
#include "osm-namefinder/NamedTagHandler.h"

#include "MarbleDebug.h"

#include "GeoDataDocument.h"
#include "GeoDataPlacemark.h"
#include "GeoParser.h"
#include "osm-namefinder/AttributeDictionary.h"
#include "osm-namefinder/ElementDictionary.h"

namespace Marble
{
namespace OsmNamefinder
{

static GeoTagHandlerRegistrar
namedTagHandler( GeoTagHandler::QualifiedName( tag_named, tag_namespace ),
         new NamedTagHandler );


GeoNode * NamedTagHandler::parse( GeoParser & parser ) const
{
    Q_ASSERT( parser.isStartElement() && parser.isValidElement( tag_named ));
    mDebug() << "NamedTagHandler";

    GeoStackItem parentItem = parser.parentElement();
    mDebug() << "parentItem:" << parentItem.qualifiedName().first;

    // FIXME: better check tags before?
    GeoDataPlacemark *named = new GeoDataPlacemark;

    // FIXME: attribute type
//     const ItemType type = getItemType( parser.attribute( attr_type ).trimmed() );
//     named->setType( type );

    // attribute id
    int id = 0;
    const QString idStr = parser.attribute( attr_id ).trimmed();
    if ( !idStr.isEmpty() )
        id = idStr.toInt();
    named->setId( id );

    // attributes lat and lon
    qreal lat = 0.0;
    const QString latStr = parser.attribute( attr_lat ).trimmed();
    if ( !latStr.isEmpty() )
        lat = latStr.toDouble();

    qreal lon = 0.0;
    const QString lonStr = parser.attribute( attr_lon ).trimmed();
    if ( !lonStr.isEmpty() )
        lon = lonStr.toDouble();
    named->setCoordinate( GeoDataPoint( lon, lat, 0.0, GeoDataPoint::Degree, 0 ));

    // attribute name
    const QString name = parser.attribute( attr_name ).trimmed();
    named->setName( name );

    // attribute rank
    int rank = 0;
    const QString rankStr = parser.attribute( attr_rank ).trimmed();
    if ( !rankStr.isEmpty() )
        rank = rankStr.toInt();
    // FIXME: where is the rank in GeoDataPlacemark?
    // named->setRank( rank );

    // attribute zoom
    int suggestedZoomLevel = 0;
    const QString zoomStr = parser.attribute( attr_zoom ).trimmed();
    if ( !zoomStr.isEmpty() )
        suggestedZoomLevel = zoomStr.toInt();
    named->setPopularityIndex( suggestedZoomLevel );

    mDebug() << "parsed named:" << named->name() << " lon:" << lon << " lat:" << lat;

    if ( parentItem.represents( tag_searchresults )) {
        mDebug() << "added via parent searchresults";
        GeoDataContainer * const parent = parentItem.nodeAs<GeoDataContainer>();
        Q_ASSERT( parent );
        parent->append( named );

    } else if ( parentItem.represents( tag_place )) {
        mDebug() << "added via parent place, not implemented";

    } else if ( parentItem.represents( tag_nearestplaces )) {
        mDebug() << "added via parent nearestplaces, not implemented";

    } else if ( parentItem.first.first.isEmpty() && !parentItem.second ) {
        // ok, it is the parentItem would be searchresults, but we
        // have here to work around the GeoParser bug
        GeoDocument * const document = parser.activeDocument();
        Q_ASSERT( document );
        GeoDataDocument * const dataDocument =
            dynamic_cast<GeoDataDocument * const>( document );
        Q_ASSERT( dataDocument );
        dataDocument->append( named );
    }

    return 0;
}

}
}
