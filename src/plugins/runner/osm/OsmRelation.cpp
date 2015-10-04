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
    OsmMember member;
    member.reference = attributes.value("ref").toLongLong();
    member.role = attributes.value("role").toString();
    member.type = attributes.value("type").toString();
    m_members << member;
}

void OsmRelation::create(GeoDataDocument *document, const OsmWays &ways, const OsmNodes &nodes) const
{
    if (!m_osmData.containsTag("type", "multipolygon")) {
        return;
    }

    QStringList const outerRoles = QStringList() << "outer" << "";
    QList<GeoDataLinearRing> outer = rings(outerRoles, ways, nodes);
    if (outer.isEmpty()) {
        return;
    } else if (outer.size() > 1) {
        /** @todo: Merge ways with common start/end, create multipolygon geometries for ones with multiple outer rings */
        mDebug() << "Polygons with " << outer.size() << " ways are not yet supported";
        return;
    }

    GeoDataPlacemark* placemark = new GeoDataPlacemark;
    placemark->setName(m_osmData.tagValue("name"));
    placemark->setOsmData(m_osmData);
    placemark->setVisualCategory(OsmPresetLibrary::determineVisualCategory(m_osmData));
    placemark->setStyle( 0 );

    GeoDataPolygon* polygon = new GeoDataPolygon;
    polygon->setOuterBoundary(outer[0]);
    // @todo: How to get the reference here?
    // placemark->osmData().addMemberReference(-1, );

    QStringList const innerRoles = QStringList() << "inner";
    QList<GeoDataLinearRing> inner = rings(innerRoles, ways, nodes);
    foreach(const GeoDataLinearRing &ring, inner) {
        // @todo: How to get the reference here?
        // placemark->osmData().addMemberReference(polygon->innerBoundaries().size(), );
        polygon->appendInnerBoundary(ring);
    }
    placemark->setGeometry(polygon);

    OsmObjectManager::registerId( m_osmData.id() );
    document->append(placemark);
}

QList<GeoDataLinearRing> OsmRelation::rings(const QStringList &roles, const OsmWays &ways, const OsmNodes &nodes) const
{
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

    /** @todo: Merge ways with common start/end nodes here, then check sanity (result should be all rings) */

    QList<GeoDataLinearRing> result;
    foreach(qint64 wayId, roleMembers) {
        GeoDataLinearRing ring;
        OsmWay const & way = ways[wayId];
        if (way.references().first() != way.references().last()) {
            // Non-trivial rings (where multiple ways have to be merged) are not yet supported. Return nothing
            mDebug() << "Ignoring non-trivial polygon boundary in relation " << m_osmData.id();
            return QList<GeoDataLinearRing>();
        }
        foreach(qint64 id, way.references()) {
            if (!nodes.contains(id)) {
                // A node is missing. Return nothing.
                return QList<GeoDataLinearRing>();
            }
            ring << nodes[id].coordinates();
        }
        result << ring;
    }
    return result;
}

}
