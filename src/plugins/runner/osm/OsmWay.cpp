// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2015 Dennis Nienhüser <nienhueser@kde.org>
//

#include <OsmRelation.h>
#include <MarbleDebug.h>
#include <GeoDataPlacemark.h>
#include <GeoDataLineStyle.h>
#include <GeoDataPolyStyle.h>
#include <GeoDataStyle.h>
#include <GeoDataDocument.h>
#include <osm/OsmObjectManager.h>
#include <MarbleDirs.h>
#include <GeoDataMultiGeometry.h>

namespace Marble {

QSet<StyleBuilder::OsmTag> OsmWay::s_areaTags;
QSet<StyleBuilder::OsmTag> OsmWay::s_buildingTags;

GeoDataPlacemark *OsmWay::create(const OsmNodes &nodes, QSet<qint64> &usedNodes) const
{
    OsmPlacemarkData osmData = m_osmData;
    GeoDataGeometry *geometry = nullptr;

    if (isArea()) {
        GeoDataLinearRing linearRing;
        linearRing.reserve(m_references.size());
        bool const stripLastNode = m_references.first() == m_references.last();
        for (int i=0, n=m_references.size() - (stripLastNode ? 1 : 0); i<n; ++i) {
            qint64 nodeId = m_references[i];
            auto const nodeIter = nodes.constFind(nodeId);
            if (nodeIter == nodes.constEnd()) {
                return nullptr;
            }

            OsmNode const & node = nodeIter.value();
            osmData.addNodeReference(node.coordinates(), node.osmData());
            linearRing.append(node.coordinates());
            usedNodes << nodeId;
        }

        if (isBuilding()) {
            GeoDataBuilding building;
            building.setName(extractBuildingName());
            building.setHeight(extractBuildingHeight());
            building.setEntries(extractNamedEntries());
            building.multiGeometry()->append(new GeoDataLinearRing(linearRing.optimized()));

            geometry = new GeoDataBuilding(building);
        } else {
            geometry = new GeoDataLinearRing(linearRing.optimized());
        }
    } else {
        GeoDataLineString lineString;
        lineString.reserve(m_references.size());

        for(auto nodeId: m_references) {
            auto const nodeIter = nodes.constFind(nodeId);
            if (nodeIter == nodes.constEnd()) {
                return nullptr;
            }

            OsmNode const & node = nodeIter.value();
            osmData.addNodeReference(node.coordinates(), node.osmData());
            lineString.append(node.coordinates());
            usedNodes << nodeId;
        }

        geometry = new GeoDataLineString(lineString.optimized());
    }

    Q_ASSERT(geometry != nullptr);

    OsmObjectManager::registerId(m_osmData.id());

    GeoDataPlacemark *placemark = new GeoDataPlacemark;
    placemark->setGeometry(geometry);
    placemark->setVisualCategory(StyleBuilder::determineVisualCategory(m_osmData));
    placemark->setName(m_osmData.tagValue(QStringLiteral("name")));
    if (placemark->name().isEmpty()) {
        placemark->setName(m_osmData.tagValue(QStringLiteral("ref")));
    }
    placemark->setOsmData(osmData);
    placemark->setZoomLevel(StyleBuilder::minimumZoomLevel(placemark->visualCategory()));
    placemark->setPopularity(StyleBuilder::popularity(placemark));
    placemark->setVisible(placemark->visualCategory() != GeoDataPlacemark::None);

    return placemark;
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

    if (m_osmData.containsTag(QStringLiteral("area"), QStringLiteral("yes"))) {
        return true;
    }

    bool const isLinearFeature =
            m_osmData.containsTag(QStringLiteral("area"), QStringLiteral("no")) ||
            m_osmData.containsTagKey(QStringLiteral("highway")) ||
            m_osmData.containsTagKey(QStringLiteral("barrier"));
    if (isLinearFeature) {
        return false;
    }

    bool const isAreaFeature = m_osmData.containsTagKey(QStringLiteral("landuse"));
    if (isAreaFeature) {
        return true;
    }

    for (auto iter = m_osmData.tagsBegin(), end=m_osmData.tagsEnd(); iter != end; ++iter) {
        const auto tag = StyleBuilder::OsmTag(iter.key(), iter.value());
        if (isAreaTag(tag)) {
            return true;
        }
    }

    bool const isImplicitlyClosed = m_references.size() > 1 && m_references.front() == m_references.last();
    return isImplicitlyClosed;
}

bool OsmWay::isAreaTag(const StyleBuilder::OsmTag &keyValue)
{
    if (s_areaTags.isEmpty()) {
        // All these tags can be found updated at
        // https://wiki.openstreetmap.org/wiki/Map_Features#Landuse

        s_areaTags.insert(StyleBuilder::OsmTag(QStringLiteral("natural"), QStringLiteral("water")));
        s_areaTags.insert(StyleBuilder::OsmTag(QStringLiteral("natural"), QStringLiteral("wood")));
        s_areaTags.insert(StyleBuilder::OsmTag(QStringLiteral("natural"), QStringLiteral("beach")));
        s_areaTags.insert(StyleBuilder::OsmTag(QStringLiteral("natural"), QStringLiteral("wetland")));
        s_areaTags.insert(StyleBuilder::OsmTag(QStringLiteral("natural"), QStringLiteral("glacier")));
        s_areaTags.insert(StyleBuilder::OsmTag(QStringLiteral("natural"), QStringLiteral("scrub")));
        s_areaTags.insert(StyleBuilder::OsmTag(QStringLiteral("natural"), QStringLiteral("cliff")));
        s_areaTags.insert(StyleBuilder::OsmTag(QStringLiteral("area"), QStringLiteral("yes")));
        s_areaTags.insert(StyleBuilder::OsmTag(QStringLiteral("waterway"), QStringLiteral("riverbank")));

        for (auto const & tag: StyleBuilder::buildingTags()) {
            s_areaTags.insert(tag);
        }
        s_areaTags.insert(StyleBuilder::OsmTag(QStringLiteral("man_made"), QStringLiteral("bridge")));

        s_areaTags.insert(StyleBuilder::OsmTag(QStringLiteral("amenity"), QStringLiteral("graveyard")));
        s_areaTags.insert(StyleBuilder::OsmTag(QStringLiteral("amenity"), QStringLiteral("parking")));
        s_areaTags.insert(StyleBuilder::OsmTag(QStringLiteral("amenity"), QStringLiteral("parking_space")));
        s_areaTags.insert(StyleBuilder::OsmTag(QStringLiteral("amenity"), QStringLiteral("bicycle_parking")));
        s_areaTags.insert(StyleBuilder::OsmTag(QStringLiteral("amenity"), QStringLiteral("college")));
        s_areaTags.insert(StyleBuilder::OsmTag(QStringLiteral("amenity"), QStringLiteral("hospital")));
        s_areaTags.insert(StyleBuilder::OsmTag(QStringLiteral("amenity"), QStringLiteral("kindergarten")));
        s_areaTags.insert(StyleBuilder::OsmTag(QStringLiteral("amenity"), QStringLiteral("school")));
        s_areaTags.insert(StyleBuilder::OsmTag(QStringLiteral("amenity"), QStringLiteral("university")));
        s_areaTags.insert(StyleBuilder::OsmTag(QStringLiteral("leisure"), QStringLiteral("common")));
        s_areaTags.insert(StyleBuilder::OsmTag(QStringLiteral("leisure"), QStringLiteral("garden")));
        s_areaTags.insert(StyleBuilder::OsmTag(QStringLiteral("leisure"), QStringLiteral("golf_course")));
        s_areaTags.insert(StyleBuilder::OsmTag(QStringLiteral("leisure"), QStringLiteral("marina")));
        s_areaTags.insert(StyleBuilder::OsmTag(QStringLiteral("leisure"), QStringLiteral("playground")));
        s_areaTags.insert(StyleBuilder::OsmTag(QStringLiteral("leisure"), QStringLiteral("pitch")));
        s_areaTags.insert(StyleBuilder::OsmTag(QStringLiteral("leisure"), QStringLiteral("park")));
        s_areaTags.insert(StyleBuilder::OsmTag(QStringLiteral("leisure"), QStringLiteral("sports_centre")));
        s_areaTags.insert(StyleBuilder::OsmTag(QStringLiteral("leisure"), QStringLiteral("stadium")));
        s_areaTags.insert(StyleBuilder::OsmTag(QStringLiteral("leisure"), QStringLiteral("swimming_pool")));
        s_areaTags.insert(StyleBuilder::OsmTag(QStringLiteral("leisure"), QStringLiteral("track")));

        s_areaTags.insert(StyleBuilder::OsmTag(QStringLiteral("military"), QStringLiteral("danger_area")));

        s_areaTags.insert(StyleBuilder::OsmTag(QStringLiteral("marble_land"), QStringLiteral("landmass")));
        s_areaTags.insert(StyleBuilder::OsmTag(QStringLiteral("settlement"), QStringLiteral("yes")));
    }

    return s_areaTags.contains(keyValue);
}

bool OsmWay::isBuilding() const
{
    for (auto iter = m_osmData.tagsBegin(), end=m_osmData.tagsEnd(); iter != end; ++iter) {
        const auto tag = StyleBuilder::OsmTag(iter.key(), iter.value());
        if (isBuildingTag(tag)) {
            return true;
        }
    }

    return false;
}

bool OsmWay::isBuildingTag(const StyleBuilder::OsmTag &keyValue)
{
    if (s_buildingTags.isEmpty()) {
        for (auto const & tag: StyleBuilder::buildingTags()) {
            s_buildingTags.insert(tag);
        }
    }

    return s_buildingTags.contains(keyValue);
}

QString OsmWay::extractBuildingName() const
{
    auto tagIter = m_osmData.findTag(QStringLiteral("addr:housename"));
    if (tagIter != m_osmData.tagsEnd()) {
        return tagIter.value();
    }

    tagIter = m_osmData.findTag(QStringLiteral("addr:housenumber"));
    if (tagIter != m_osmData.tagsEnd()) {
        return tagIter.value();
    }

    return QString();
}

double OsmWay::extractBuildingHeight() const
{
    double height = 8.0;

    QHash<QString, QString>::const_iterator tagIter;
    if ((tagIter = m_osmData.findTag(QStringLiteral("height"))) != m_osmData.tagsEnd()) {
        height = GeoDataBuilding::parseBuildingHeight(tagIter.value());
    } else if ((tagIter = m_osmData.findTag(QStringLiteral("building:levels"))) != m_osmData.tagsEnd()) {
        int const levels = tagIter.value().toInt();
        int const skipLevels = m_osmData.tagValue(QStringLiteral("building:min_level")).toInt();
        /** @todo Is 35 as an upper bound for the number of levels sane? */
        height = 3.0 * qBound(1, 1+levels-skipLevels, 35);
    }

    return qBound(1.0, height, 1000.0);
}

QVector<GeoDataBuilding::NamedEntry> OsmWay::extractNamedEntries() const
{
    QVector<GeoDataBuilding::NamedEntry> entries;

    const auto end = m_osmData.nodeReferencesEnd();
    for (auto iter = m_osmData.nodeReferencesBegin(); iter != end; ++iter) {
        const auto tagIter = iter.value().findTag(QStringLiteral("addr:housenumber"));
        if (tagIter != iter.value().tagsEnd()) {
            GeoDataBuilding::NamedEntry entry;
            entry.point = iter.key();
            entry.label = tagIter.value();
            entries.push_back(entry);
        }
    }

    return entries;
}

}
