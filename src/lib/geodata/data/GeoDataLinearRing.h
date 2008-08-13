//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2008 Torsten Rahn <tackat@kde.org>
//


#ifndef GEODATALINEARRING_H
#define GEODATALINEARRING_H


#include "geodata_export.h"
#include "GeoDataLineString.h"


namespace Marble
{

/**
 * @short A class that defines a closed, contiguous set of line segments.
 *
 * Also known as: closed Polyline
 *
 * GeoDataLinearRing consists of GeoDataPoints connected through line 
 * segments.
 * For convenience and performance we've added special methods 
 * to calculate the smallest GeoDataLatLonAltBox that contains the
 * GeoDataLinearRing.
 * This class will at some point replace GeoPolygon which is
 * currently used for a very similar purpose.
 */

class GEODATA_EXPORT GeoDataLinearRing : public GeoDataLineString {
 public:
    GeoDataLinearRing();

    virtual ~GeoDataLinearRing();
    virtual EnumGeometryId geometryId() const { return GeoDataLinearRingId; };
};

}

#endif // GEODATALINEARRING_H
