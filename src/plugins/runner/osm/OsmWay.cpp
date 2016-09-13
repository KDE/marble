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
#include <osm/OsmObjectManager.h>
#include <MarbleDirs.h>
#include <StyleBuilder.h>

namespace Marble {

QSet<QString> OsmWay::s_areaTags;

void OsmWay::create(GeoDataDocument *document, const OsmNodes &nodes, QSet<qint64> &usedNodes) const
{
    GeoDataPlacemark* placemark = new GeoDataPlacemark;
    placemark->setOsmData(m_osmData);
    placemark->setVisualCategory(StyleBuilder::determineVisualCategory(m_osmData));
    placemark->setName(m_osmData.tagValue(QStringLiteral("name")));
    if (placemark->name().isEmpty()) {
        placemark->setName(m_osmData.tagValue(QStringLiteral("ref")));
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
        QString const keyValue = iter.key() + QLatin1Char('=') + iter.value();
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

        s_areaTags.insert(QStringLiteral("natural=water"));
        s_areaTags.insert(QStringLiteral("natural=wood"));
        s_areaTags.insert(QStringLiteral("natural=beach"));
        s_areaTags.insert(QStringLiteral("natural=wetland"));
        s_areaTags.insert(QStringLiteral("natural=glacier"));
        s_areaTags.insert(QStringLiteral("natural=scrub"));
        s_areaTags.insert(QStringLiteral("natural=cliff"));
        s_areaTags.insert(QStringLiteral("area=yes"));
        s_areaTags.insert(QStringLiteral("waterway=riverbank"));

        foreach(const QString &value, StyleBuilder::buildingValues() ) {
            s_areaTags.insert(QLatin1String("building=") + value);
        }
        s_areaTags.insert(QStringLiteral("man_made=bridge"));

        s_areaTags.insert(QStringLiteral("amenity=graveyard"));
        s_areaTags.insert(QStringLiteral("amenity=parking"));
        s_areaTags.insert(QStringLiteral("amenity=parking_space"));
        s_areaTags.insert(QStringLiteral("amenity=bicycle_parking"));
        s_areaTags.insert(QStringLiteral("amenity=college"));
        s_areaTags.insert(QStringLiteral("amenity=hospital"));
        s_areaTags.insert(QStringLiteral("amenity=kindergarten"));
        s_areaTags.insert(QStringLiteral("amenity=school"));
        s_areaTags.insert(QStringLiteral("amenity=university"));
        s_areaTags.insert(QStringLiteral("leisure=common"));
        s_areaTags.insert(QStringLiteral("leisure=garden"));
        s_areaTags.insert(QStringLiteral("leisure=golf_course"));
        s_areaTags.insert(QStringLiteral("leisure=marina"));
        s_areaTags.insert(QStringLiteral("leisure=playground"));
        s_areaTags.insert(QStringLiteral("leisure=pitch"));
        s_areaTags.insert(QStringLiteral("leisure=park"));
        s_areaTags.insert(QStringLiteral("leisure=sports_centre"));
        s_areaTags.insert(QStringLiteral("leisure=stadium"));
        s_areaTags.insert(QStringLiteral("leisure=swimming_pool"));
        s_areaTags.insert(QStringLiteral("leisure=track"));

        s_areaTags.insert(QStringLiteral("military=danger_area"));

        s_areaTags.insert(QStringLiteral("marble_land=landmass"));
        s_areaTags.insert(QStringLiteral("settlement=yes"));
    }

    return s_areaTags.contains(keyValue);
}

}
