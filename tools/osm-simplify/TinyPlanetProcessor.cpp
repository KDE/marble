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
#include "OsmObjectManager.h"

#include <QDebug>
#include <QPolygonF>
#include <QPair>

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
    clipper.initClipRect(tileBoundary, 20);

    foreach (GeoDataPlacemark* placemark, placemarks()) {

        if(tileBoundary.intersects(placemark->geometry()->latLonAltBox())) {

            if( placemark->geometry()->nodeType() == GeoDataTypes::GeoDataPolygonType) {
                GeoDataPolygon* marblePolygon = static_cast<GeoDataPolygon*>(placemark->geometry());
                int index = -1;

                using PolygonPair = QPair<GeoDataPlacemark*, QPolygonF>;
                QVector<PolygonPair> newMarblePolygons;

                bool const isClockwise = marblePolygon->outerBoundary().isClockwise();
                QPolygonF outerBoundaryPolygon = BaseClipper::toQPolygon(marblePolygon->outerBoundary(), !isClockwise);

                QVector<QPolygonF> outerBoundaries;
                clipper.clipPolyObject(outerBoundaryPolygon, outerBoundaries, true);

                //                qDebug() << "Size(s) after:";
                foreach(const QPolygonF& polygon, outerBoundaries) {

                    //                    qDebug() << polygon.size();

                    PolygonPair newMarblePolygon = qMakePair(new GeoDataPlacemark(), polygon);
                    GeoDataPolygon* geometry = new GeoDataPolygon();
                    geometry->setOuterBoundary(BaseClipper::toLineString<GeoDataLinearRing>(polygon, !isClockwise));
                    newMarblePolygon.first->setGeometry(geometry);

                    copyTags(*placemark, *(newMarblePolygon.first));
                    OsmObjectManager::initializeOsmData(newMarblePolygon.first);

                    placemark->osmData().memberReference(index);
                    copyTags(placemark->osmData().memberReference(index),
                             newMarblePolygon.first->osmData().memberReference(index));

                    newMarblePolygons.push_back(newMarblePolygon);
                }

                foreach (const GeoDataLinearRing& innerBoundary, marblePolygon->innerBoundaries()) {
                    ++index;
                    bool const isClockwise = innerBoundary.isClockwise();
                    QPolygonF innerBoundaryPolygon = BaseClipper::toQPolygon(innerBoundary, !isClockwise);

                    QVector<QPolygonF> clippedPolygons;

                    clipper.clipPolyObject(innerBoundaryPolygon, clippedPolygons, true);

                    foreach (const QPolygonF& polygon, clippedPolygons) {
                        bool isAdded = false;
                        foreach (const PolygonPair& newMarblePolygon, newMarblePolygons) {
                            if(!polygon.intersected(newMarblePolygon.second).isEmpty()) {
                                GeoDataPolygon* geometry = static_cast<GeoDataPolygon*>(newMarblePolygon.first->geometry());
                                geometry->appendInnerBoundary(BaseClipper::toLineString<GeoDataLinearRing>(polygon, !isClockwise));

                                OsmObjectManager::initializeOsmData(newMarblePolygon.first);

                                OsmPlacemarkData& innerRingData = newMarblePolygon.first->osmData().memberReference(geometry->innerBoundaries().size()-1);
                                OsmPlacemarkData& placemarkInnerRingData = placemark->osmData().memberReference(index);

                                copyTags(placemarkInnerRingData, innerRingData);

                                isAdded = true;
                            }
                        }

                        if(!isAdded) {
                            qDebug() << "Polygon not added. Why?";
                        }
                    }
                }

                foreach (const PolygonPair& newMarblePolygon, newMarblePolygons) {
                    tile->append(newMarblePolygon.first);
                }

            } else if (placemark->geometry()->nodeType() == GeoDataTypes::GeoDataLineStringType) {
                GeoDataLineString* marbleWay = static_cast<GeoDataLineString*>(placemark->geometry());

                QVector<QPolygonF> clippedPolygons;

                QPolygonF way = BaseClipper::toQPolygon(*marbleWay);

                clipper.clipPolyObject(way, clippedPolygons, false);

                //                qDebug() << "Size  before:" << way.size();
                //                qDebug() << "Size(s) after:";
                foreach(const QPolygonF& polygon, clippedPolygons) {

                    //                    qDebug() << polygon.size();

                    GeoDataLineString* newMarbleWay = new GeoDataLineString(BaseClipper::toLineString<GeoDataLineString>(polygon));

                    GeoDataPlacemark* newPlacemark = new GeoDataPlacemark();
                    newPlacemark->setGeometry(newMarbleWay);
                    copyTags(*placemark, *newPlacemark);

                    tile->append(newPlacemark);
                }
            } else if (placemark->geometry()->nodeType() == GeoDataTypes::GeoDataLinearRingType) {

                GeoDataLinearRing* marbleClosedWay = static_cast<GeoDataLinearRing*>(placemark->geometry());

                QVector<QPolygonF> clippedPolygons;

                QPolygonF closedWay = BaseClipper::toQPolygon(*marbleClosedWay);

                clipper.clipPolyObject(closedWay, clippedPolygons, true);

                //                    qDebug() << "Size(s) after:";
                foreach(const QPolygonF& polygon, clippedPolygons) {

                    //                        qDebug() << polygon.size();

                    GeoDataLinearRing* newMarbleWay = new GeoDataLinearRing(BaseClipper::toLineString<GeoDataLinearRing>(polygon));

                    GeoDataPlacemark* newPlacemark = new GeoDataPlacemark();
                    newPlacemark->setGeometry(newMarbleWay);
                    copyTags(*placemark, *newPlacemark);

                    tile->append(newPlacemark);
                }

            } else {
                tile->append(placemark);
            }
        }
    }


    return tile;
}

void TinyPlanetProcessor::copyTags(const GeoDataPlacemark &source, GeoDataPlacemark &target) const
{
    copyTags(source.osmData(), target.osmData());
}

void TinyPlanetProcessor::copyTags(const OsmPlacemarkData &originalPlacemarkData, OsmPlacemarkData &targetOsmData) const
{
    for (auto iter=originalPlacemarkData.tagsBegin(), end=originalPlacemarkData.tagsEnd(); iter != end; ++iter) {
        targetOsmData.addTag(iter.key(), iter.value());
    }
}
