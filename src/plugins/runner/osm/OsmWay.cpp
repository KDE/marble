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


void OsmWay::create(GeoDataDocument *document, const OsmNodes &nodes) const
{
    bool const shouldRender =
        !m_osmData.containsTag("boundary", "postal_code") &&
        !m_osmData.containsTagKey("closed:highway") &&
        !m_osmData.containsTagKey("abandoned:highway") &&
        !m_osmData.containsTagKey("abandoned:natural") &&
        !m_osmData.containsTagKey("abandoned:building") &&
        !m_osmData.containsTagKey("abandoned:leisure") &&
        !m_osmData.containsTagKey("disused:highway") &&
        !m_osmData.containsTag("highway", "razed");

    GeoDataPlacemark* placemark = new GeoDataPlacemark;
    placemark->setOsmData(m_osmData);
    placemark->setVisualCategory(OsmPresetLibrary::determineVisualCategory(m_osmData));
    placemark->setName(m_osmData.tagValue("name"));
    placemark->setVisible(shouldRender);

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
        }

        *linearRing = linearRing->optimized();
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
    for (auto iter = m_osmData.tagsBegin(), end=m_osmData.tagsEnd(); iter != end; ++iter) {
        QString const keyValue = QString("%1=%2").arg(iter.key()).arg(iter.value());
        if (OsmPresetLibrary::isAreaTag(keyValue)) {
            return true;
        }
    }

    return false;
}

}
