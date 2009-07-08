//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2008-2009      Patrick Spendrin <ps_ml@gmx.de>
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

namespace Marble
{

class GeoDataPolygonPrivate;

/**
 * @short A class that defines a closed, contiguous set of line segments.
 *        In addition to a LineString the Polygon can contain "holes"
 *
 * Also known as: Polygon
 *
 * GeoDataPolygon consists of GeoDataCoordinates connected through line 
 * segments.
 * For convenience and performance we've added special methods 
 * to calculate the smallest GeoDataLatLonAltBox that contains the
 * GeoDataPolygon.
 */

class GEODATA_EXPORT GeoDataPolygon : public GeoDataGeometry {
 public:
    explicit GeoDataPolygon( TessellationFlags f = Tessellate );
    GeoDataPolygon( const GeoDataGeometry &other );

    virtual ~GeoDataPolygon();

    virtual bool isClosed() const;

    bool tessellate() const;
    void setTessellate( bool tessellate );

    TessellationFlags tessellationFlags() const;
    void setTessellationFlags( TessellationFlags f );

    GeoDataLatLonAltBox latLonAltBox() const;

    GeoDataLinearRing& outerBoundary() const;
    void setOuterBoundary( const GeoDataLinearRing& boundary );

    QVector<GeoDataLinearRing>& innerBoundaries() const;
    void appendInnerBoundary( const GeoDataLinearRing& boundary );

    /// Serialize the contents of the feature to @p stream.
    virtual void pack( QDataStream& stream ) const;
    /// Unserialize the contents of the feature from @p stream.
    virtual void unpack( QDataStream& stream );

 protected:
    GeoDataPolygonPrivate *p() const;
};

class GEODATA_EXPORT GeoDataOuterBoundary : public GeoDataPolygon {};
class GEODATA_EXPORT GeoDataInnerBoundary : public GeoDataPolygon {};

}

#endif // GEODATAPOLYGON_H
