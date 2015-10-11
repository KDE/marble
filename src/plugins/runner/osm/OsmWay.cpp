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
    GeoDataPlacemark* placemark = new GeoDataPlacemark;
    placemark->setOsmData(m_osmData);
    placemark->setName(m_osmData.tagValue("name"));
    placemark->setVisualCategory(OsmPresetLibrary::determineVisualCategory(m_osmData));
    placemark->setStyle( 0 );

    if (isArea()) {
        GeoDataLinearRing* linearRing = new GeoDataLinearRing;
        placemark->setGeometry(linearRing);

        foreach(qint64 nodeId, m_references) {
            if (!nodes.contains(nodeId)) {
                delete placemark;
                return;
            }

            OsmNode const & node = nodes[nodeId];
            placemark->osmData().addNodeReference(node.coordinates(), node.osmData());
            linearRing->append(node.coordinates());
        }

        if(placemark->visualCategory() == GeoDataFeature::AmenityGraveyard ||
                 placemark->visualCategory() == GeoDataFeature::LanduseCemetery) {
            bool adjustStyle = true;
            GeoDataPolyStyle polyStyle = placemark->style()->polyStyle();
            if( m_osmData.containsTag("religion","jewish") ){
                polyStyle.setTexturePath(MarbleDirs::path("bitmaps/osmcarto/patterns/grave_yard_jewish.png"));
            } else if( m_osmData.containsTag("religion","christian") ){
                polyStyle.setTexturePath(MarbleDirs::path("bitmaps/osmcarto/patterns/grave_yard_christian.png"));
            } else if( m_osmData.containsTag("religion","INT-generic") ){
                polyStyle.setTexturePath(MarbleDirs::path("bitmaps/osmcarto/patterns/grave_yard_generic.png"));
            } else {
                adjustStyle = false;
            }
            if (adjustStyle) {
                GeoDataStyle* style = new GeoDataStyle(*placemark->style());
                style->setPolyStyle(polyStyle);
                placemark->setStyle(style);
            }
        }

        QList<GeoDataFeature::GeoDataVisualCategory> categories = OsmPresetLibrary::visualCategories(m_osmData);
        foreach(GeoDataFeature::GeoDataVisualCategory category, categories) {
            GeoDataStyle* categoryStyle = GeoDataFeature::presetStyle(category);
            if (!categoryStyle->iconStyle().iconPath().isEmpty()) {
                GeoDataStyle* style = new GeoDataStyle(*placemark->style());
                style->setIconStyle(categoryStyle->iconStyle());
                placemark->setStyle(style);
            }
        }
    } else {
        GeoDataLineString* lineString = new GeoDataLineString;
        placemark->setGeometry(lineString);

        foreach(qint64 nodeId, m_references) {
            if (!nodes.contains(nodeId)) {
                delete placemark;
                return;
            }

            OsmNode const & node = nodes[nodeId];
            placemark->osmData().addNodeReference(node.coordinates(), node.osmData());
            lineString->append(node.coordinates());
        }

        if (placemark->visualCategory() > GeoDataFeature::HighwayService &&
                placemark->visualCategory() <= GeoDataFeature::HighwayMotorway) {
            bool const isOneWay = m_osmData.containsTag("oneway", "yes") || m_osmData.containsTag("oneway", "-1");
            int const lanes = isOneWay ? 1 : 2; // also for motorway which implicitly is one way, but has two lanes and each direction has its own highway
            double const laneWidth = 3.0;
            double const margins = placemark->visualCategory() == GeoDataFeature::HighwayMotorway ? 2.0 : (isOneWay ? 1.0 : 0.0);
            double const physicalWidth = margins + lanes * laneWidth;

            GeoDataLineStyle lineStyle = placemark->style()->lineStyle();
            lineStyle.setPhysicalWidth(physicalWidth);
            lineStyle.setCosmeticOutline(true);
            GeoDataStyle* style = new GeoDataStyle(*placemark->style());
            style->setLineStyle(lineStyle);
            placemark->setStyle(style);
        }
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
