// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2016 Friedrich W. H. Kossebau <kossebau@kde.org>

#include "GeoUriRunner.h"

#include "GeoDataCoordinates.h"
#include "GeoDataPlacemark.h"
#include "GeoUriParser.h"
#include "MarbleModel.h"

#include "MarbleDebug.h"
#include <QList>

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
    QList<GeoDataPlacemark *> vector;

    GeoUriParser uriParser(searchTerm);
    const bool success = uriParser.parse();
    if (success && (uriParser.planet().id() == model()->planet()->id())) {
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

#include "moc_GeoUriRunner.cpp"
