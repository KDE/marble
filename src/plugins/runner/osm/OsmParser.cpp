//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011      Thibaut Gridel <tgridel@free.fr>
// Copyright 2011      Konstantin Oblaukhov <oblaukhov.konstantin@gmail.com>
// Copyright 2014      Bernhard Beschow <bbeschow@cs.tu-berlin.de>
//

#include "OsmParser.h"
#include "OsmElementDictionary.h"
#include "osm/OsmPresetLibrary.h"
#include "osm/OsmObjectManager.h"
#include "GeoDataDocument.h"
#include "GeoDataPlacemark.h"
#include "GeoDataPoint.h"
#include "GeoDataTypes.h"
#include "GeoDataStyle.h"

namespace Marble {

const QColor OsmParser::backgroundColor( 0xF1, 0xEE, 0xE8 );

OsmParser::OsmParser()
    : GeoParser( 0 )
{
    // nothing to do
}

OsmParser::~OsmParser()
{
    qDeleteAll( m_dummyPlacemarks );
    qDeleteAll( m_nodes );
}

void OsmParser::setNode( qint64 id, GeoDataPoint *point )
{
    m_nodes[id] = point;
}

GeoDataPoint *OsmParser::node( qint64 id )
{
    return m_nodes.value( id );
}

void OsmParser::setWay( qint64 id, GeoDataLineString *way )
{
    m_ways[id] = way;
}

GeoDataLineString *OsmParser::way( qint64 id )
{
    return m_ways.value( id );
}

void OsmParser::setPolygon( qint64 id, GeoDataPolygon *polygon )
{
    m_polygons[id] = polygon;
}

GeoDataPolygon *OsmParser::polygon( qint64 id )
{
    return m_polygons.value( id );
}

void OsmParser::addDummyPlacemark( GeoDataPlacemark *placemark )
{
    m_dummyPlacemarks << placemark;
}

void OsmParser::adjustStyles(GeoDataDocument* document)
{
    for (int i=0, n=document->size(); i<n; ++i) {
        GeoDataFeature* feature = document->child(i);
        if (feature->nodeType() == GeoDataTypes::GeoDataPlacemarkType) {
            GeoDataPlacemark* placemark = static_cast<GeoDataPlacemark*>(feature);
            if (isHighway(placemark)) {
                calculateHighwayWidth(placemark);
            }
            else if( placemark->visualCategory() == GeoDataFeature::AmenityGraveyard ||
                     placemark->visualCategory() == GeoDataFeature::LanduseCemetery ){
                adjustGraveyardPattern( placemark );
            }
        }
    }
}

bool OsmParser::isValidRootElement()
{
    return isValidElement(osm::osmTag_osm);
}

bool OsmParser::isHighway(const GeoDataPlacemark *placemark) const
{
    return placemark->visualCategory() >= GeoDataFeature::HighwaySteps &&
           placemark->visualCategory() <= GeoDataFeature::HighwayMotorway;
}

void OsmParser::calculateHighwayWidth(GeoDataPlacemark *placemark) const
{
    if (placemark->visualCategory() >= GeoDataFeature::HighwaySteps &&
        placemark->visualCategory() <= GeoDataFeature::HighwayService) {
        return;
    }

    OsmPlacemarkData const & data = placemark->osmData();
    bool const isOneWay = data.containsTag("oneway", "yes") || data.containsTag("oneway", "-1");
    int const lanes = isOneWay ? 1 : 2; // also for motorway which implicitly is one way, but has two lanes and each direction has its own highway
    double const laneWidth = 3.0;
    double const margins = placemark->visualCategory() == GeoDataFeature::HighwayMotorway ? 2.0 : (isOneWay ? 1.0 : 0.0);
    double const physicalWidth = margins + lanes * laneWidth;

    GeoDataLineStyle lineStyle = placemark->style()->lineStyle();
    lineStyle.setPhysicalWidth(physicalWidth);
    GeoDataStyle* style = new GeoDataStyle(*placemark->style());
    style->setLineStyle(lineStyle);
    placemark->setStyle(style);
}

void OsmParser::adjustGraveyardPattern(GeoDataPlacemark *placemark) const
{
    OsmPlacemarkData const & data = placemark->osmData();
    GeoDataPolyStyle polyStyle = placemark->style()->polyStyle();
    if( data.containsTag("religion","jewish") ){
        polyStyle.setTexturePath(MarbleDirs::path("bitmaps/osmcarto/patterns/grave_yard_jewish.png"));
    } else if( data.containsTag("religion","christian") ){
        polyStyle.setTexturePath(MarbleDirs::path("bitmaps/osmcarto/patterns/grave_yard_christian.png"));
    } else if( data.containsTag("religion","INT-generic") ){
        polyStyle.setTexturePath(MarbleDirs::path("bitmaps/osmcarto/patterns/grave_yard_generic.png"));
    } else {
        return;
    }
    GeoDataStyle* style = new GeoDataStyle(*placemark->style());
    style->setPolyStyle(polyStyle);
    placemark->setStyle(style);
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

GeoDataDocument *OsmXmlParser::parse(const QString &filename, QString &error)
{
    QFile file(filename);
    if (!file.open(QFile::ReadOnly)) {
        error = QString("Cannot open file %1").arg(filename);
        return nullptr;
    }

    QXmlStreamReader parser;
    parser.setDevice(&file);
    OsmPlacemarkData* osmData(0);
    QString parentTag;
    qint64 parentId(0);

    OsmNodes m_nodes;
    OsmWays m_ways;
    OsmRelations m_relations;

    while (!parser.atEnd()) {
        parser.readNext();
        if (!parser.isStartElement()) {
            continue;
        }

        QStringRef const tagName = parser.name();
        if (tagName == osm::osmTag_node || tagName == osm::osmTag_way || tagName == osm::osmTag_relation) {
            parentTag = parser.name().toString();
            parentId = parser.attributes().value("id").toLongLong();

            if (tagName == osm::osmTag_node) {
                m_nodes[parentId].osmData() = OsmPlacemarkData::fromParserAttributes(parser.attributes());
                m_nodes[parentId].parseCoordinates(parser.attributes());
                osmData = &m_nodes[parentId].osmData();
            } else if (tagName == osm::osmTag_way) {
                m_ways[parentId].osmData() = OsmPlacemarkData::fromParserAttributes(parser.attributes());
                osmData = &m_ways[parentId].osmData();
            } else {
                Q_ASSERT(tagName == osm::osmTag_relation);
                m_relations[parentId].osmData() = OsmPlacemarkData::fromParserAttributes(parser.attributes());
                osmData = &m_relations[parentId].osmData();
            }
        } else if (tagName == osm::osmTag_tag) {
            osmData->addTag(parser.attributes().value("k").toString(), parser.attributes().value("v").toString());
        } else if (tagName == osm::osmTag_nd && parentTag == osm::osmTag_way) {
            m_ways[parentId].addReference(parser.attributes().value("ref").toLongLong());
        } else if (tagName == osm::osmTag_member && parentTag == osm::osmTag_relation) {
            m_relations[parentId].parseMember(parser.attributes());
        } // other tags like osm, bounds ignored
    }

    if (parser.hasError()) {
        error = parser.errorString();
        return nullptr;
    }

    return createDocument(m_nodes, m_ways, m_relations);
}

GeoDataDocument *OsmXmlParser::createDocument(OsmNodes &nodes, OsmWays &ways, OsmRelations &relations)
{
    GeoDataDocument* document = new GeoDataDocument;
    GeoDataPolyStyle backgroundPolyStyle;
    backgroundPolyStyle.setFill( true );
    backgroundPolyStyle.setOutline( false );
    backgroundPolyStyle.setColor("#f1eee8");
    GeoDataStyle backgroundStyle;
    backgroundStyle.setPolyStyle( backgroundPolyStyle );
    backgroundStyle.setId( "background" );
    document->addStyle( backgroundStyle );

    foreach(OsmRelation const &relation, relations) {
        relation.create(document, ways, nodes);
    }

    foreach(OsmWay const &way, ways) {
        way.create(document, nodes);
    }

    foreach(OsmNode const &node, nodes) {
        node.create(document);
    }
    return document;
}

}
