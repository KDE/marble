//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2008 Henry de Valence <hdevalence@gmail.com>

#include "LatLonRunner.h"

#include "GeoDataPlacemark.h"
#include "GeoDataCoordinates.h"

#include "MarbleDebug.h"
#include <QString>
#include <QVector>

#include <QtDebug>

namespace Marble
{

LatLonRunner::LatLonRunner(QObject *parent) :
    SearchRunner(parent)
{
}


LatLonRunner::~LatLonRunner()
{
}

void LatLonRunner::search( const QString &searchTerm, const GeoDataLatLonBox & )
{
    QVector<GeoDataPlacemark*> vector;

    bool successful = false;
    const GeoDataCoordinates coord = GeoDataCoordinates::fromString( searchTerm, successful );

    if( successful ) {
        GeoDataPlacemark *placemark = new GeoDataPlacemark;
        placemark->setName( searchTerm );
        qreal lon, lat;
        coord.geoCoordinates( lon, lat );
        placemark->setCoordinate( lon, lat );
        placemark->setVisualCategory(GeoDataPlacemark::Coordinate);
        placemark->setPopularity( 1000000000 );
        placemark->setZoomLevel( 1 );
        vector.append( placemark );
    }

    emit searchFinished( vector );
}

}

#include "moc_LatLonRunner.cpp"
