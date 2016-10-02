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
#include <GeoDataDocument.h>
#include <GeoDataPolygon.h>
#include <StyleBuilder.h>
#include <osm/OsmObjectManager.h>

namespace Marble {

OsmRelation::OsmMember::OsmMember() :
    reference(0)
{
    // nothing to do
}

OsmPlacemarkData &OsmRelation::osmData()
{
    return m_osmData;
}

const OsmPlacemarkData &OsmRelation::osmData() const
{
    return m_osmData;
}

void OsmRelation::parseMember(const QXmlStreamAttributes &attributes)
{
    addMember(attributes.value(QLatin1String("ref")).toLongLong(),
              attributes.value(QLatin1String("role")).toString(),
              attributes.value(QLatin1String("type")).toString());
}

void OsmRelation::addMember(qint64 reference, const QString &role, const QString &type)
{
    OsmMember member;
    member.reference = reference;
    member.role = role;
    member.type = type;
    m_members << member;
}

void OsmRelation::create(GeoDataDocument *document, OsmWays &ways, const OsmNodes &nodes, QSet<qint64> &usedNodes, QSet<qint64> &usedWays) const
{
    if (!m_osmData.containsTag(QStringLiteral("type"), QStringLiteral("multipolygon"))) {
        return;
    }

    QStringList const outerRoles = QStringList() << QStringLiteral("outer") << QString();
    QSet<qint64> outerWays;
    QSet<qint64> outerNodes;
    const QList<GeoDataLinearRing> outer = rings(outerRoles, ways, nodes, outerNodes, outerWays);

    if (outer.isEmpty()) {
        return;
    }

    GeoDataPlacemark::GeoDataVisualCategory outerCategory = StyleBuilder::determineVisualCategory(m_osmData);
    if (outerCategory == GeoDataPlacemark::None) {
        // Try to determine the visual category from the relation members
        GeoDataPlacemark::GeoDataVisualCategory const firstCategory =
                StyleBuilder::determineVisualCategory(ways[*outerWays.begin()].osmData());

        bool categoriesAreSame = true;
        foreach (auto wayId, outerWays) {
            GeoDataPlacemark::GeoDataVisualCategory const category =
                    StyleBuilder::determineVisualCategory(ways[wayId].osmData());
            if( category != firstCategory ) {
                categoriesAreSame = false;
                break;
            }
        }

        if( categoriesAreSame ) {
            outerCategory = firstCategory;
        }
    }

    foreach(qint64 wayId, outerWays) {
        Q_ASSERT(ways.contains(wayId));
        GeoDataPlacemark::GeoDataVisualCategory const category = StyleBuilder::determineVisualCategory(ways[wayId].osmData());
        if (category == GeoDataPlacemark::None || category == outerCategory) {
            // Schedule way for removal: It's a non-styled way only used to create the outer boundary in this polygon
            usedWays << wayId;
        } // else we keep it

        foreach(qint64 nodeId, ways[wayId].references()) {
            ways[wayId].osmData().addNodeReference(nodes[nodeId].coordinates(), nodes[nodeId].osmData());
        }
    }

    QStringList const innerRoles = QStringList() << QStringLiteral("inner");
    QSet<qint64> innerWays;
    const QList<GeoDataLinearRing> inner = rings(innerRoles, ways, nodes, usedNodes, innerWays);

    OsmPlacemarkData osmData = m_osmData;
    osmData.addMemberReference(-1, ways[*outerWays.begin()].osmData());
    int index = 0;
    foreach(qint64 wayId, innerWays) {
        Q_ASSERT(ways.contains(wayId));
        if (StyleBuilder::determineVisualCategory(ways[wayId].osmData()) == GeoDataPlacemark::None) {
            // Schedule way for removal: It's a non-styled way only used to create the inner boundary in this polygon
            usedWays << wayId;
        }
        foreach(qint64 nodeId, ways[wayId].references()) {
            ways[wayId].osmData().addNodeReference(nodes[nodeId].coordinates(), nodes[nodeId].osmData());
        }
        osmData.addMemberReference(index, ways[wayId].osmData());
        ++index;
    }

    if (outerCategory == GeoDataPlacemark::Bathymetry) {
        // In case of a bathymetry store elevation info since it is required during styling
        // The ele=* tag is present in the outermost way
        const QString ele = QStringLiteral("ele");
        const OsmPlacemarkData &outerWayData = ways[*outerWays.begin()].osmData();
        auto tagIter = outerWayData.findTag(ele);
        if (tagIter != outerWayData.tagsEnd()) {
            osmData.addTag(ele, tagIter.value());
        }
    }

    bool const hasMultipleOuterRings = outer.size() > 1;
    foreach(auto const &outerRing, outer) {
        GeoDataPolygon *polygon = new GeoDataPolygon;
        polygon->setOuterBoundary(outerRing);
        foreach(const GeoDataLinearRing &innerRing, inner) {
            if (!innerRing.isEmpty() && outerRing.contains(innerRing.first())) {
                polygon->appendInnerBoundary(innerRing);
            }
        }

        GeoDataPlacemark *placemark = new GeoDataPlacemark;
        placemark->setName(m_osmData.tagValue(QStringLiteral("name")));
        placemark->setVisualCategory(outerCategory);
        placemark->setStyle( GeoDataStyle::Ptr() );
        placemark->setVisible(outerCategory != GeoDataPlacemark::None);
        placemark->setGeometry(polygon);
        if (hasMultipleOuterRings) {
            /** @TODO Use a GeoDataMultiGeometry to keep the ID? */
            osmData.setId(0);
            OsmObjectManager::initializeOsmData(placemark);
        } else {
            OsmObjectManager::registerId(osmData.id());
        }
        placemark->setOsmData(osmData);
        usedNodes |= outerNodes;

        document->append(placemark);
    }
}

QList<GeoDataLinearRing> OsmRelation::rings(const QStringList &roles, const OsmWays &ways, const OsmNodes &nodes, QSet<qint64> &usedNodes, QSet<qint64> &usedWays) const
{
    QSet<qint64> currentWays;
    QSet<qint64> currentNodes;
    QList<qint64> roleMembers;
    foreach(const OsmMember &member, m_members) {
        if (roles.contains(member.role)) {
            if (!ways.contains(member.reference)) {
                // A way is missing. Return nothing.
                return QList<GeoDataLinearRing>();
            }
            roleMembers << member.reference;
        }
    }

    QList<GeoDataLinearRing> result;
    QList<OsmWay> unclosedWays;
    foreach(qint64 wayId, roleMembers) {
        GeoDataLinearRing ring;
        OsmWay const & way = ways[wayId];
        if (way.references().first() != way.references().last()) {
            unclosedWays.append(way);
            continue;
        }
        foreach(qint64 id, way.references()) {
            if (!nodes.contains(id)) {
                // A node is missing. Return nothing.
                return QList<GeoDataLinearRing>();
            }
            ring << nodes[id].coordinates();
        }
        Q_ASSERT(ways.contains(wayId));
        currentWays << wayId;
        result << ring;
    }

    if( !unclosedWays.isEmpty() ) {
        //mDebug() << "Trying to merge non-trivial polygon boundary in relation " << m_osmData.id();
        while( unclosedWays.length() > 0 ) {
            GeoDataLinearRing ring;
            qint64 firstReference = unclosedWays.first().references().first();
            qint64 lastReference = firstReference;
            bool ok = true;
            while( ok ) {
                ok = false;
                for(int i = 0; i<unclosedWays.length(); ) {
                    const OsmWay &nextWay = unclosedWays.at(i);
                    if( nextWay.references().first() == lastReference
                            || nextWay.references().last() == lastReference ) {

                        bool isReversed = nextWay.references().last() == lastReference;
                        QVector<qint64> v = nextWay.references();
                        while( !v.isEmpty() ) {
                            qint64 id = isReversed ? v.takeLast() : v.takeFirst();
                            if (!nodes.contains(id)) {
                                // A node is missing. Return nothing.
                                return QList<GeoDataLinearRing>();
                            }
                            if ( id != lastReference ) {
                                ring << nodes[id].coordinates();
                                currentNodes << id;
                            }
                        }
                        lastReference = isReversed ? nextWay.references().first()
                                                   : nextWay.references().last();
                        Q_ASSERT(ways.contains(nextWay.osmData().id()));
                        currentWays << nextWay.osmData().id();
                        unclosedWays.removeAt(i);
                        ok = true;
                        break;
                    } else {
                        ++i;
                    }
                }
            }

            if(lastReference != firstReference) {
                return QList<GeoDataLinearRing>();
            } else {
                result << ring;
            }
        }
    }

    usedWays |= currentWays;
    usedNodes |= currentNodes;
    return result;
}

}
