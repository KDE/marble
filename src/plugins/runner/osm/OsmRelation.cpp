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
#include <osm/OsmPresetLibrary.h>
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
    addMember(attributes.value("ref").toLongLong(),
              attributes.value("role").toString(),
              attributes.value("type").toString());
}

void OsmRelation::addMember(qint64 reference, const QString &role, const QString &type)
{
    OsmMember member;
    member.reference = reference;
    member.role = role;
    member.type = type;
    m_members << member;
}

void OsmRelation::create(GeoDataDocument *document, const OsmWays &ways, const OsmNodes &nodes, QSet<qint64> &usedWays) const
{
    if (!m_osmData.containsTag("type", "multipolygon")) {
        return;
    }

    QStringList const outerRoles = QStringList() << "outer" << "";
    QSet<qint64> outerWays;
    QList<GeoDataLinearRing> outer = rings(outerRoles, ways, nodes, outerWays);
    if (outer.isEmpty()) {
        return;
    } else if (outer.size() > 1) {
        /** @todo: Merge ways with common start/end, create multipolygon geometries for ones with multiple outer rings */
        mDebug() << "Polygons with " << outer.size() << " ways are not yet supported";
        return;
    }
    GeoDataFeature::GeoDataVisualCategory outerCategory = OsmPresetLibrary::determineVisualCategory(m_osmData);
    if (outerCategory == GeoDataFeature::None) {
        // Try to determine the visual category from the relation members
        bool categoriesAreSame = true;
        auto iterator = outerWays.begin();
        GeoDataFeature::GeoDataVisualCategory const firstCategory =
                OsmPresetLibrary::determineVisualCategory(ways[*iterator].osmData());
        for( ; iterator != outerWays.end(); ++iterator ) {
            GeoDataFeature::GeoDataVisualCategory const category =
                    OsmPresetLibrary::determineVisualCategory(ways[*iterator].osmData());
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
        GeoDataFeature::GeoDataVisualCategory const category = OsmPresetLibrary::determineVisualCategory(ways[wayId].osmData());
        if (category == GeoDataFeature::None || category == outerCategory) {
            // Schedule way for removal: It's a non-styled way only used to create the outer boundary in this polygon
            usedWays << wayId;
        } // else we keep it
    }

    if (m_osmData.containsTag("historic", "castle") && m_osmData.containsTag("castle_type", "kremlin")) {
        outerCategory = GeoDataFeature::None;
    }

    GeoDataPlacemark* placemark = new GeoDataPlacemark;
    placemark->setName(m_osmData.tagValue("name"));
    placemark->setOsmData(m_osmData);
    placemark->setVisualCategory(outerCategory);
    placemark->setStyle( GeoDataStyle::Ptr() );

    GeoDataPolygon* polygon = new GeoDataPolygon;
    polygon->setOuterBoundary(outer[0]);
    // @todo: How to get the reference here?
    // placemark->osmData().addMemberReference(-1, );

    QStringList const innerRoles = QStringList() << "inner";
    QSet<qint64> innerWays;
    QList<GeoDataLinearRing> inner = rings(innerRoles, ways, nodes, innerWays);
    foreach(qint64 wayId, innerWays) {
        Q_ASSERT(ways.contains(wayId));
        if (OsmPresetLibrary::determineVisualCategory(ways[wayId].osmData()) == GeoDataFeature::None) {
            // Schedule way for removal: It's a non-styled way only used to create the inner boundary in this polygon
            usedWays << wayId;
        }
    }
    foreach(const GeoDataLinearRing &ring, inner) {
        // @todo: How to get the reference here?
        // placemark->osmData().addMemberReference(polygon->innerBoundaries().size(), );
        polygon->appendInnerBoundary(ring);
    }
    placemark->setGeometry(polygon);

    OsmObjectManager::registerId( m_osmData.id() );
    document->append(placemark);
}

QList<GeoDataLinearRing> OsmRelation::rings(const QStringList &roles, const OsmWays &ways, const OsmNodes &nodes, QSet<qint64> &usedWays) const
{
    QSet<qint64> currentWays;
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
    return result;
}

}
