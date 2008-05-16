//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2008 Torsten Rahn <tackat@kde.org>"
//


#ifndef GEODATALINESTRING_H
#define GEODATALINESTRING_H

#include <QtCore/QVector>

#include "global.h"

#include "geodata_export.h"
#include "GeoDataGeometry.h"
#include "GeoDataLatLonAltBox.h"


using namespace Marble;


class GeoDataLineStringPrivate;

/**
 * @short A class that defines a contiguous set of line segments.
 *
 * Also known as: non-closed Polyline
 *
 * GeoDataLineString consists of GeoDataPoints connected through line 
 * segments.
 * For convenience and performance we've added special methods 
 * to calculate the smallest GeoDataLatLonAltBox that contains the
 * GeoDataLineString.
 * This class will at some point replace GeoPolygon which is
 * currently used for a very similar purpose.
 */

class GEODATA_EXPORT GeoDataLineString : public QVector<GeoDataPoint>,
                                         public GeoDataGeometry {
 public:
    GeoDataLineString();

    virtual ~GeoDataLineString();

    bool extrude() const;
    void setExtrude( bool extrude );

    bool tesselate() const;
    void setTesselate( bool tesselate );

    AltitudeMode altitudeMode() const;
    void setAltitudeMode( const AltitudeMode altitudeMode );

    GeoDataLatLonAltBox latLonAltBox() const;

    void append ( const GeoDataPoint & value );
    void clear();

    typedef QVector<GeoDataPoint> Vector;

    QVector<GeoDataPoint>::Iterator erase ( QVector<GeoDataPoint>::Iterator pos );
    QVector<GeoDataPoint>::Iterator erase ( QVector<GeoDataPoint>::Iterator begin,
                                            QVector<GeoDataPoint>::Iterator end );

 private:
    GeoDataLineStringPrivate  * const d;
};

#endif // GEODATALINESTRING_H
