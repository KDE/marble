//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011 Thibaut Gridel <tgridel@free.fr>
// Copyright 2011      Konstantin Oblaukhov <oblaukhov.konstantin@gmail.com>
// Copyright 2014      Bernhard Beschow <bbeschow@cs.tu-berlin.de>
//

#include "OsmParser.h"
#include "OsmElementDictionary.h"
#include "GeoDataDocument.h"

namespace Marble {

const QColor OsmParser::backgroundColor( 0xF1, 0xEE, 0xE8 );

OsmParser::OsmParser()
    : GeoParser( 0 )
{
    // All these tags can be found updated at
    // http://wiki.openstreetmap.org/wiki/Map_Features#Landuse

    m_areaTags.insert( "landuse=forest" );
    m_areaTags.insert( "natural=wood" );
    m_areaTags.insert( "area=yes" );
    m_areaTags.insert( "waterway=riverbank" );
    m_areaTags.insert( "building=yes" );
    m_areaTags.insert( "amenity=parking" );
    m_areaTags.insert( "leisure=park" );

    m_areaTags.insert( "landuse=allotments" );
    m_areaTags.insert( "landuse=basin" );
    m_areaTags.insert( "landuse=brownfield" );
    m_areaTags.insert( "landuse=cemetery" );
    m_areaTags.insert( "landuse=commercial" );
    m_areaTags.insert( "landuse=construction" );
    m_areaTags.insert( "landuse=farm" );
    m_areaTags.insert( "landuse=farmland" );
    m_areaTags.insert( "landuse=farmyard" );
    m_areaTags.insert( "landuse=garages" );
    m_areaTags.insert( "landuse=greenfield" );
    m_areaTags.insert( "landuse=industrial" );
    m_areaTags.insert( "landuse=landfill" );
    m_areaTags.insert( "landuse=meadow" );
    m_areaTags.insert( "landuse=military" );
    m_areaTags.insert( "landuse=orchard" );
    m_areaTags.insert( "landuse=quarry" );
    m_areaTags.insert( "landuse=railway" );
    m_areaTags.insert( "landuse=reservoir" );
    m_areaTags.insert( "landuse=residential" );
    m_areaTags.insert( "landuse=retail" );
}

OsmParser::~OsmParser()
{
    qDeleteAll( m_dummyPlacemarks );
    qDeleteAll( m_nodes );
}

void OsmParser::setNode( quint64 id, GeoDataPoint *point )
{
    m_nodes[id] = point;
}

GeoDataPoint *OsmParser::node( quint64 id )
{
    return m_nodes.value( id );
}

void OsmParser::setWay( quint64 id, GeoDataLineString *way )
{
    m_ways[id] = way;
}

GeoDataLineString *OsmParser::way( quint64 id )
{
    return m_ways.value( id );
}

void OsmParser::setPolygon( quint64 id, GeoDataPolygon *polygon )
{
    m_polygons[id] = polygon;
}

GeoDataPolygon *OsmParser::polygon( quint64 id )
{
    return m_polygons.value( id );
}

bool OsmParser::tagNeedArea( const QString &keyValue ) const
{
    return m_areaTags.contains( keyValue );
}

void OsmParser::addDummyPlacemark( GeoDataPlacemark *placemark )
{
    m_dummyPlacemarks << placemark;
}

bool OsmParser::isValidRootElement()
{
    return isValidElement(osm::osmTag_osm);
}

bool OsmParser::isValidElement(const QString& tagName) const
{
    if (!GeoParser::isValidElement(tagName))
        return false;

    //always "valid" because there is no namespace
    return true;
}

GeoDocument* OsmParser::createDocument() const
{
    return new GeoDataDocument;
}

}
