//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2016      David Kolozsvari <freedawson@gmail.com>
//

#include "TinyPlanetProcessor.h"

#include "BaseClipper.h"

#include "GeoDataPlacemark.h"
#include "OsmPlacemarkData.h"

#include <QDebug>
#include <QPolygonF>

TinyPlanetProcessor::TinyPlanetProcessor(GeoDataDocument* document) :
    PlacemarkFilter(document)
{

}

void TinyPlanetProcessor::process()
{
    // ?
}

GeoDataDocument *TinyPlanetProcessor::cutToTiles(unsigned int zoomLevel, unsigned int tileX, unsigned int tileY)
{
    unsigned int N = pow(2, zoomLevel);

    GeoDataDocument* tile = new GeoDataDocument();
    QString tileName = QString("%1/%2/%3").arg(zoomLevel).arg(tileX).arg(tileY);
    tile->setName(tileName);

    GeoDataLatLonBox tileBoundary;
    qreal north = BaseClipper::tileY2lat(tileY, N);
    qreal south = BaseClipper::tileY2lat(tileY+1, N);
    qreal west = BaseClipper::tileX2lon(tileX, N);
    qreal east = BaseClipper::tileX2lon(tileX+1, N);

    tileBoundary.setBoundaries(north, south, east, west);

    BaseClipper clipper;
    clipper.initClipRect(tileBoundary);

    foreach (GeoDataPlacemark* placemark, m_placemarks) {

        if(tileBoundary.intersects(placemark->geometry()->latLonAltBox())) {

            if( placemark->geometry()->nodeType() == GeoDataTypes::GeoDataPolygonType ||
                placemark->visualCategory() == GeoDataFeature::Landmass) {

                GeoDataLinearRing* marblePolygon;
                if(placemark->geometry()->nodeType() == GeoDataTypes::GeoDataPolygonType) {
                    marblePolygon = &static_cast<GeoDataPolygon*>(placemark->geometry())->outerBoundary();
                } else if (placemark->geometry()->nodeType() == GeoDataTypes::GeoDataLinearRingType) {
                    marblePolygon = static_cast<GeoDataLinearRing*>(placemark->geometry());
                }

                QVector<QPolygonF> clippedPolygons;

                QPolygonF outerBoundary = BaseClipper::linearRing2Qpolygon(*marblePolygon);

                clipper.clipPolyObject(outerBoundary, clippedPolygons, true);

                foreach(const QPolygonF& polygon, clippedPolygons) {

                    GeoDataLinearRing outerBoundary = BaseClipper::qPolygon2linearRing(polygon);
                    GeoDataPolygon* newMarblePolygon = new GeoDataPolygon();
                    newMarblePolygon->setOuterBoundary(outerBoundary);

                    GeoDataPlacemark* newPlacemark = new GeoDataPlacemark();
                    newPlacemark->setGeometry(newMarblePolygon);
                    newPlacemark->setVisualCategory(GeoDataFeature::Landmass);

                    OsmPlacemarkData marbleLand;
                    marbleLand.addTag("marble_land","landmass");
                    newPlacemark->setOsmData(marbleLand);

                    tile->append(newPlacemark);
                }
            } else if (placemark->geometry()->nodeType() == GeoDataTypes::GeoDataLineStringType) {
                GeoDataLineString* marbleWay = static_cast<GeoDataLineString*>(placemark->geometry());

                QVector<QPolygonF> clippedPolygons;

                QPolygonF way = BaseClipper::lineString2Qpolygon(*marbleWay);

                clipper.clipPolyObject(way, clippedPolygons, false);

                foreach(const QPolygonF& polygon, clippedPolygons) {

                    GeoDataLineString* newMarbleWay = new GeoDataLineString(BaseClipper::qPolygon2lineString(polygon));

                    GeoDataPlacemark* newPlacemark = new GeoDataPlacemark();
                    newPlacemark->setGeometry(newMarbleWay);
                    newPlacemark->setVisualCategory(placemark->visualCategory());

                    OsmPlacemarkData osmData;
                    auto it = placemark->osmData().tagsBegin();
                    auto itEnd = placemark->osmData().tagsEnd();
                    while(it != itEnd) {
                        osmData.addTag(it.key(), it.value());
                        ++it;
                    }
                    newPlacemark->setOsmData(osmData);

                    tile->append(newPlacemark);
                }
            } else if (placemark->geometry()->nodeType() == GeoDataTypes::GeoDataLinearRingType) {

                    GeoDataLinearRing* marbleClosedWay = static_cast<GeoDataLinearRing*>(placemark->geometry());

                    QVector<QPolygonF> clippedPolygons;

                    QPolygonF closedWay = BaseClipper::linearRing2Qpolygon(*marbleClosedWay);

                    // If we cut a closed way to pieces, the results shouldn't be closed ways too
                    clipper.clipPolyObject(closedWay, clippedPolygons, false);

                    foreach(const QPolygonF& polygon, clippedPolygons) {

                        // When a linearRing is cut to pieces, the resulting geometries will be lineStrings
                        GeoDataLineString* newMarbleWay = new GeoDataLineString(BaseClipper::qPolygon2lineString(polygon));

                        GeoDataPlacemark* newPlacemark = new GeoDataPlacemark();
                        newPlacemark->setGeometry(newMarbleWay);
                        newPlacemark->setVisualCategory(placemark->visualCategory());

                        OsmPlacemarkData osmData;
                        auto it = placemark->osmData().tagsBegin();
                        auto itEnd = placemark->osmData().tagsEnd();
                        while(it != itEnd) {
                            osmData.addTag(it.key(), it.value());
                            ++it;
                        }
                        newPlacemark->setOsmData(osmData);

                        tile->append(newPlacemark);
                    }

            } else {
                tile->append(placemark);
            }
        }
    }


    return tile;
}
