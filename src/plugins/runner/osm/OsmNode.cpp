//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2015      Dennis Nienhüser <nienhueser@kde.org>
//

#include <OsmNode.h>

#include "osm/OsmPresetLibrary.h"
#include "osm/OsmObjectManager.h"
#include <GeoDataPlacemark.h>
#include <GeoDataStyle.h>
#include <GeoDataIconStyle.h>
#include <MarbleDirs.h>
#include <QDate>

namespace Marble {

void OsmNode::parseCoordinates(const QXmlStreamAttributes &attributes)
{
    qreal const lon = attributes.value( "lon" ).toDouble();
    qreal const lat = attributes.value( "lat" ).toDouble();
    m_coordinates = GeoDataCoordinates(lon, lat, 0, GeoDataCoordinates::Degree);
}

void OsmNode::create(GeoDataDocument *document) const
{
    GeoDataFeature::GeoDataVisualCategory const category = OsmPresetLibrary::determineVisualCategory(m_osmData);
    if (category == GeoDataFeature::None ||
       (category >= GeoDataFeature::HighwaySteps && category <= GeoDataFeature::HighwayMotorway)) {
        return;
    }

    GeoDataPlacemark* placemark = new GeoDataPlacemark;
    placemark->setOsmData(m_osmData);
    placemark->setCoordinate(m_coordinates);

    if ((category == GeoDataFeature::TransportCarShare || category == GeoDataFeature::MoneyAtm)
            && m_osmData.containsTagKey("operator")) {
        placemark->setName(m_osmData.tagValue("operator"));
    } else {
        placemark->setName(m_osmData.tagValue("name"));
    }
    placemark->setZoomLevel( 18 );
    placemark->setVisualCategory(category);
    placemark->setStyle( 0 );

    if (category == GeoDataFeature::NaturalTree) {
        qreal const lat = m_coordinates.latitude(GeoDataCoordinates::Degree);
        if (qAbs(lat) > 15) {
            /** @todo Should maybe auto-adjust to MarbleClock at some point */
            QDate const date = QDate::currentDate();
            bool const southernHemisphere = lat < 0;
            QDate const autumnStart = QDate(date.year(), southernHemisphere ? 3 : 9, 15);
            QDate const winterEnd = southernHemisphere ? QDate(date.year(), 8, 15) : QDate(date.year()+1, 2, 15);
            if (date > autumnStart && date < winterEnd) {
                QDate const autumnEnd = QDate(date.year(), southernHemisphere ? 5 : 11, 15);
                QString const season = date < autumnEnd ? "autumn" : "winter";
                GeoDataIconStyle iconStyle = placemark->style()->iconStyle();
                QString const bitmap = QString("bitmaps/osmcarto/symbols/48/individual/tree-29-%1.png").arg(season);
                iconStyle.setIconPath(MarbleDirs::path(bitmap));

                GeoDataStyle* style = new GeoDataStyle(*placemark->style());
                style->setIconStyle(iconStyle);
                placemark->setStyle(style);

            }
        }
    }

    OsmObjectManager::registerId(m_osmData.id());
    document->append(placemark);
}

GeoDataCoordinates OsmNode::coordinates() const
{
    return m_coordinates;
}

OsmPlacemarkData &OsmNode::osmData()
{
    return m_osmData;
}

const OsmPlacemarkData &OsmNode::osmData() const
{
    return m_osmData;
}

}
