//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2008      Patrick Spendrin <ps_ml@gmx.de>
// Copyright 2008      Inge Wallin <inge@lysator.liu.se>
//


#ifndef GEODATAPOLYGON_H
#define GEODATAPOLYGON_H

#include <QtCore/QVector>

#include "global.h"

#include "geodata_export.h"
#include "GeoDataGeometry.h"
#include "GeoDataLinearRing.h"
#include "GeoDataLatLonAltBox.h"


using namespace Marble;


class GeoDataPolygonPrivate;

/**
 * @short A class that defines a contiguous set of line segments.
 *
 * Also known as: non-closed Polyline
 *
 * GeoDataPolygon consists of GeoDataCoordinates connected through line 
 * segments.
 * For convenience and performance we've added special methods 
 * to calculate the smallest GeoDataLatLonAltBox that contains the
 * GeoDataPolygon.
 * This class will at some point replace GeoPolygon which is
 * currently used for a very similar purpose.
 */

class GEODATA_EXPORT GeoDataPolygon : public GeoDataGeometry {
 public:
    GeoDataPolygon();
    GeoDataPolygon( const GeoDataPolygon & );
    GeoDataPolygon& operator=( const GeoDataPolygon & );

    virtual ~GeoDataPolygon();

    GeoDataLatLonAltBox latLonAltBox() const;

    GeoDataLinearRing& outerBoundary() const;
    void setOuterBoundary( GeoDataLinearRing* boundary );

    QVector<GeoDataLinearRing*> innerBoundaries() const;
    void appendInnerBoundary( GeoDataLinearRing* boundary );

    /// Serialize the contents of the feature to @p stream.
    virtual void pack( QDataStream& stream ) const;
    /// Unserialize the contents of the feature from @p stream.
    virtual void unpack( QDataStream& stream );

    virtual EnumGeometryId geometryId() const { return GeoDataPolygonId; };
 protected:
    GeoDataPolygonPrivate * const d;
};

class GEODATA_EXPORT GeoDataOuterBoundary : public GeoDataPolygon {};
class GEODATA_EXPORT GeoDataInnerBoundary : public GeoDataPolygon {};
#endif // GEODATAPOLYGON_H
