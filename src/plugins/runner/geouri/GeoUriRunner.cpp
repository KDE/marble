//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2016  Friedrich W. H. Kossebau <kossebau@kde.org>

#include "GeoUriRunner.h"

#include "GeoDataPlacemark.h"
#include "GeoDataCoordinates.h"
#include "GeoUriParser.h"
#include "MarbleModel.h"

#include "MarbleDebug.h"
#include <QVector>


namespace Marble
{

GeoUriRunner::GeoUriRunner(QObject *parent)
    : SearchRunner(parent)
{
}


GeoUriRunner::~GeoUriRunner()
{
}

void GeoUriRunner::search(const QString &searchTerm, const GeoDataLatLonBox &)
{
    QVector<GeoDataPlacemark*> vector;

    GeoUriParser uriParser(searchTerm);
    const bool success = uriParser.parse();
    if (success &&
        (uriParser.planet().id() == model()->planet()->id())) {
        const GeoDataCoordinates coordinates = uriParser.coordinates();

        GeoDataPlacemark *placemark = new GeoDataPlacemark;
        placemark->setName(searchTerm);
        placemark->setCoordinate(coordinates);
        placemark->setVisualCategory(GeoDataPlacemark::Coordinate);
        placemark->setPopularity(1000000000);
        placemark->setZoomLevel(1);

        vector.append(placemark);
    }

    emit searchFinished(vector);
}

}
