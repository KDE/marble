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
#include "GeoDataCoordinates.h"
#include "GeoDataLatLonAltBox.h"


using namespace Marble;


class GeoDataLineStringPrivate;

/**
 * @short A class that defines a contiguous set of line segments.
 *
 * Also known as: non-closed Polyline
 *
 * GeoDataLineString consists of GeoDataCoordinates connected through line 
 * segments.
 * For convenience and performance we've added special methods 
 * to calculate the smallest GeoDataLatLonAltBox that contains the
 * GeoDataLineString.
 * This class will at some point replace GeoPolygon which is
 * currently used for a very similar purpose.
 */

class GEODATA_EXPORT GeoDataLineString : public QVector<GeoDataCoordinates*>,
                                         public GeoDataGeometry {
 public:
    GeoDataLineString();
    GeoDataLineString( const GeoDataLineString & );
    GeoDataLineString& operator=( const GeoDataLineString & );

    virtual ~GeoDataLineString();

    GeoDataLatLonAltBox latLonAltBox() const;

    void append ( GeoDataCoordinates* value );
    void clear();

    QVector<GeoDataCoordinates*>::Iterator erase ( QVector<GeoDataCoordinates*>::Iterator pos );
    QVector<GeoDataCoordinates*>::Iterator erase ( QVector<GeoDataCoordinates*>::Iterator begin,
                                                   QVector<GeoDataCoordinates*>::Iterator end );

 protected:
    GeoDataLineStringPrivate  * const d;
};

#endif // GEODATALINESTRING_H
