//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2016      David Kolozsvari <freedawson@gmail.com>
//

#include "ShpCoastlineProcessor.h"

#include "BaseClipper.h"

#include "GeoDataPlacemark.h"
#include "OsmPlacemarkData.h"

#include <QPolygonF>
#include <QDebug>

ShpCoastlineProcessor::ShpCoastlineProcessor(GeoDataDocument* document) :
    PlacemarkFilter(document)
{

}

void ShpCoastlineProcessor::process()
{
    OsmPlacemarkData marbleLand;
    marbleLand.addTag("marble_land","landmass");

    foreach (GeoDataPlacemark* placemark, placemarks()) {
        if(placemark->geometry()->nodeType() == GeoDataTypes::GeoDataPolygonType) {
            placemark->setOsmData(marbleLand);
        }
    }
}

GeoDataDocument *ShpCoastlineProcessor::cutToTiles(unsigned int zoomLevel, unsigned int tileX, unsigned int tileY)
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

    //            qDebug() << tileName;
    //            qDebug() << "west:  " << west*RAD2DEG <<  "\t east:  " << east*RAD2DEG;
    //            qDebug() << "north: " << north*RAD2DEG << "\t south: " << south*RAD2DEG;
    //            qDebug() << "\n";

    tileBoundary.setBoundaries(north, south, east, west);

    foreach (GeoDataPlacemark* placemark, placemarks()) {

        if(placemark->geometry()->nodeType() == GeoDataTypes::GeoDataPolygonType) {
            GeoDataPolygon* marblePolygon = static_cast<GeoDataPolygon*>(placemark->geometry());

            if(tileBoundary.intersects(marblePolygon->latLonAltBox())) {
                BaseClipper clipper;
                clipper.initClipRect(tileBoundary, 20);

                QVector<QPolygonF> clippedPolygons;

                QPolygonF outerBoundary = BaseClipper::linearRing2Qpolygon(marblePolygon->outerBoundary());
                qDebug() << "Size before:" << outerBoundary.size();

                qDebug() << "Clipping...";
                clipper.clipPolyObject(outerBoundary, clippedPolygons, true);
                qDebug() << "done.";

                qDebug() << "Number of polygons after clipping: " << clippedPolygons.size();
                qDebug() << "Size(s) after:";
                foreach(const QPolygonF& polygon, clippedPolygons) {
                    qDebug() << polygon.size();

                    GeoDataLinearRing outerBoundary = BaseClipper::qPolygon2linearRing(polygon);
                    GeoDataPolygon* newMarblePolygon = new GeoDataPolygon();
                    newMarblePolygon->setOuterBoundary(outerBoundary);

                    GeoDataPlacemark* newPlacemark = new GeoDataPlacemark();
                    newPlacemark->setGeometry(newMarblePolygon);
                    newPlacemark->setVisualCategory(GeoDataFeature::Landmass);
                    newPlacemark->setOsmData(placemark->osmData());

                    tile->append(newPlacemark);
                }
                qDebug() << "";
            }
        }
    }


    return tile;
}
