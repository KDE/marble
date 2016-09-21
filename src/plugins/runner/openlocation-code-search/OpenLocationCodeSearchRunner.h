//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2015      Constantin Mihalache <mihalache.c94@gmail.com>
//

#ifndef OPENLOCATIONCODESEARCHRUNNER_H
#define OPENLOCATIONCODESEARCHRUNNER_H

#include "SearchRunner.h"
#include "GeoDataLatLonBox.h"
#include "MarbleDebug.h"

namespace Marble
{

class GeoDataPolygon;

class OpenLocationCodeSearchRunner : public SearchRunner
{
    Q_OBJECT
public:
    explicit OpenLocationCodeSearchRunner( QObject *parent = 0 );

    void search( const QString &searchTerm, const GeoDataLatLonBox &preferred );

private:

    /**
     * Makes a GeoDataPolygon which contains the boundary points of the
     * boundingBox parameter.
     */
    GeoDataPolygon polygonFromLatLonBox( const GeoDataLatLonBox boundingBox ) const;

    /**
     * Decode a valid open location code string into a GeoDataLatLonBox.
     */
    GeoDataLatLonBox decodeOLC( const QString &ocl ) const;

    /**
     * Decide if the string is a valid & full open location code.
     */
    bool isValidOLC( const QString &ocl ) const;

    /**
     * Maps each character from the accepted chars to its corresponding index
     * inside the string.
     */
    QHash<QChar, int> charIndex;
};

}

#endif // OPENLOCATIONCODESEARCHRUNNER_H
