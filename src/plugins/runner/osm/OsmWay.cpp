//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2015      Dennis Nienhüser <nienhueser@kde.org>
//

#include <OsmRelation.h>
#include <MarbleDebug.h>
#include <GeoDataPlacemark.h>
#include <GeoDataLineStyle.h>
#include <GeoDataPolyStyle.h>
#include <GeoDataStyle.h>
#include <osm/OsmPresetLibrary.h>
#include <osm/OsmObjectManager.h>
#include <MarbleDirs.h>

namespace Marble {

QSet<QString> OsmWay::s_areaTags;

void OsmWay::create(GeoDataDocument *document, const OsmNodes &nodes, QSet<qint64> &usedNodes) const
{
    GeoDataPlacemark* placemark = new GeoDataPlacemark;
    placemark->setOsmData(m_osmData);
    placemark->setVisualCategory(OsmPresetLibrary::determineVisualCategory(m_osmData));
    placemark->setName(m_osmData.tagValue("name"));
    if (placemark->name().isEmpty() && placemark->visualCategory() >= GeoDataFeature::HighwaySteps && placemark->visualCategory() <= GeoDataFeature::HighwayMotorway) {
        placemark->setName(m_osmData.tagValue("ref"));
    }
    placemark->setVisible(placemark->visualCategory() != GeoDataFeature::None);

    if (isArea()) {
        GeoDataLinearRing* linearRing = new GeoDataLinearRing;
        placemark->setGeometry(linearRing);

        foreach(qint64 nodeId, m_references) {
            auto const nodeIter = nodes.constFind(nodeId);
            if (nodeIter == nodes.constEnd()) {
                delete placemark;
                return;
            }

            OsmNode const & node = nodeIter.value();
            placemark->osmData().addNodeReference(node.coordinates(), node.osmData());
            linearRing->append(node.coordinates());
            usedNodes << nodeId;
        }

        *linearRing = GeoDataLinearRing(linearRing->optimized());
    } else {
        GeoDataLineString* lineString = new GeoDataLineString;
        placemark->setGeometry(lineString);

        foreach(qint64 nodeId, m_references) {
            auto const nodeIter = nodes.constFind(nodeId);
            if (nodeIter == nodes.constEnd()) {
                delete placemark;
                return;
            }

            OsmNode const & node = nodeIter.value();
            placemark->osmData().addNodeReference(node.coordinates(), node.osmData());
            lineString->append(node.coordinates());
            usedNodes << nodeId;
        }

        *lineString = lineString->optimized();
    }

    OsmObjectManager::registerId(m_osmData.id());
    document->append(placemark);
}

const QVector<qint64> &OsmWay::references() const
{
    return m_references;
}

OsmPlacemarkData &OsmWay::osmData()
{
    return m_osmData;
}

const OsmPlacemarkData &OsmWay::osmData() const
{
    return m_osmData;
}

void OsmWay::addReference(qint64 id)
{
    m_references << id;
}

bool OsmWay::isArea() const
{
    // @TODO A single OSM way can be both closed and non-closed, e.g. landuse=grass with barrier=fence.
    // We need to create two separate ways in cases like that to support this.
    // See also https://wiki.openstreetmap.org/wiki/Key:area

    bool const isLinearFeature =
            m_osmData.containsTag("area", "no") ||
            m_osmData.containsTagKey("highway") ||
            m_osmData.containsTagKey("barrier");
    if (isLinearFeature) {
        return false;
    }

    bool const isAreaFeature = m_osmData.containsTagKey("landuse");
    if (isAreaFeature) {
        return true;
    }

    for (auto iter = m_osmData.tagsBegin(), end=m_osmData.tagsEnd(); iter != end; ++iter) {
        QString const keyValue = QString("%1=%2").arg(iter.key()).arg(iter.value());
        if (isAreaTag(keyValue)) {
            return true;
        }
    }

    bool const isImplicitlyClosed = m_references.size() > 1 && m_references.front() == m_references.last();
    return isImplicitlyClosed;
}

bool OsmWay::isAreaTag(const QString &keyValue)
{
    if (s_areaTags.isEmpty()) {
        // All these tags can be found updated at
        // http://wiki.openstreetmap.org/wiki/Map_Features#Landuse

        s_areaTags.insert( "natural=water" );
        s_areaTags.insert( "natural=wood" );
        s_areaTags.insert( "natural=beach" );
        s_areaTags.insert( "natural=wetland" );
        s_areaTags.insert( "natural=glacier" );
        s_areaTags.insert( "natural=scrub" );
        s_areaTags.insert( "natural=cliff" );
        s_areaTags.insert( "area=yes" );
        s_areaTags.insert( "waterway=riverbank" );

        foreach(const QString &value, OsmPresetLibrary::buildingValues() ) {
            s_areaTags.insert( QString("building=%1").arg(value) );
        }
        s_areaTags.insert( "man_made=bridge" );

        s_areaTags.insert( "amenity=graveyard" );
        s_areaTags.insert( "amenity=parking" );
        s_areaTags.insert( "amenity=parking_space" );
        s_areaTags.insert( "amenity=bicycle_parking" );
        s_areaTags.insert( "amenity=college" );
        s_areaTags.insert( "amenity=hospital" );
        s_areaTags.insert( "amenity=kindergarten" );
        s_areaTags.insert( "amenity=school" );
        s_areaTags.insert( "amenity=university" );
        s_areaTags.insert( "leisure=common" );
        s_areaTags.insert( "leisure=garden" );
        s_areaTags.insert( "leisure=golf_course" );
        s_areaTags.insert( "leisure=marina" );
        s_areaTags.insert( "leisure=playground" );
        s_areaTags.insert( "leisure=pitch" );
        s_areaTags.insert( "leisure=park" );
        s_areaTags.insert( "leisure=sports_centre" );
        s_areaTags.insert( "leisure=stadium" );
        s_areaTags.insert( "leisure=swimming_pool" );
        s_areaTags.insert( "leisure=track" );

        s_areaTags.insert( "military=danger_area" );

        s_areaTags.insert( "marble_land=landmass" );
        s_areaTags.insert( "settlement=yes" );
    }

    return s_areaTags.contains(keyValue);
}

}
