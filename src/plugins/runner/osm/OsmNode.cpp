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
    setCoordinates(GeoDataCoordinates(lon, lat, 0, GeoDataCoordinates::Degree));
}

void OsmNode::setCoordinates(const GeoDataCoordinates &coordinates)
{
    m_coordinates = coordinates;
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
    placemark->setVisualCategory(category);
    placemark->setStyle( GeoDataStyle::Ptr() );

    if (category == GeoDataFeature::NaturalTree) {
        qreal const lat = m_coordinates.latitude(GeoDataCoordinates::Degree);
        if (qAbs(lat) > 15) {
            /** @todo Should maybe auto-adjust to MarbleClock at some point */
            int const month = QDate::currentDate().month();
            QString season;
            bool const southernHemisphere = lat < 0;
            if (southernHemisphere) {
                if (month >= 3 && month <= 5) {
                    season = "autumn";
                } else if (month >= 6 && month <= 8) {
                    season = "winter";
                }
            } else {
                if (month >= 9 && month <= 11) {
                    season = "autumn";
                } else if (month == 12 || month == 1 || month == 2) {
                    season = "winter";
                }
            }

            if (!season.isEmpty()) {
                GeoDataIconStyle iconStyle = placemark->style()->iconStyle();
                QString const bitmap = QString("bitmaps/osmcarto/symbols/48/individual/tree-29-%1.png").arg(season);
                iconStyle.setIconPath(MarbleDirs::path(bitmap));

                GeoDataStyle::Ptr style(new GeoDataStyle(*placemark->style()));
                style->setIconStyle(iconStyle);
                placemark->setStyle(style);
            }
        }
    }

    placemark->setZoomLevel( 18 );
    if (category >= GeoDataFeature::PlaceCity && category <= GeoDataFeature::PlaceVillage) {
        int const population = m_osmData.tagValue("population").toInt();
        placemark->setPopulation(qMax(0, population));
        if (population > 0) {
            placemark->setZoomLevel(populationIndex(population));
            placemark->setPopularity(population);
        } else {
            switch (category) {
            case GeoDataFeature::PlaceCity:     placemark->setZoomLevel(9);  break;
            case GeoDataFeature::PlaceSuburb:   placemark->setZoomLevel(13);  break;
            case GeoDataFeature::PlaceHamlet:   placemark->setZoomLevel(15);  break;
            case GeoDataFeature::PlaceLocality: placemark->setZoomLevel(15);  break;
            case GeoDataFeature::PlaceTown:     placemark->setZoomLevel(11);  break;
            case GeoDataFeature::PlaceVillage:  placemark->setZoomLevel(13); break;
            default:                            placemark->setZoomLevel(10); break;
            }
        }
    } else if (category == GeoDataFeature::NaturalPeak) {
        placemark->setZoomLevel(11);
        bool isInteger = false;
        int const elevation = m_osmData.tagValue("ele").toInt(&isInteger);
        if (isInteger) {
            placemark->setName(QString("%1 (%2 m)").arg(placemark->name()).arg(elevation));
        }
    }

    OsmObjectManager::registerId(m_osmData.id());
    document->append(placemark);
}

int OsmNode::populationIndex(qint64 population) const
{
    int popidx = 3;

    if ( population < 2500 )        popidx=10;
    else if ( population < 5000)    popidx=9;
    else if ( population < 25000)   popidx=8;
    else if ( population < 75000)   popidx=7;
    else if ( population < 250000)  popidx=6;
    else if ( population < 750000)  popidx=5;
    else if ( population < 2500000) popidx=4;

    return popidx;
}

const GeoDataCoordinates &OsmNode::coordinates() const
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
