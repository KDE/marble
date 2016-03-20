//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2014       Gábor Péterffy  <peterffy95@gmail.org>
//

#ifndef MARBLE_AZIMUTHALPROJECTION_H
#define MARBLE_AZIMUTHALPROJECTION_H

/** @file
 * This file contains the headers for AzimuthalProjection.
 *
 */

#include "AbstractProjection.h"

namespace Marble
{

class AzimuthalProjectionPrivate;

/**
 * @short A base class for the Gnomonic and Orthographic (Globe) projections in Marble
 */

class AzimuthalProjection : public AbstractProjection
{
    // Not a QObject so far because we don't need to send signals.
  public:

    AzimuthalProjection();

    virtual ~AzimuthalProjection();

    virtual bool repeatableX() const { return false; }
    virtual qreal  maxValidLat() const;
    virtual qreal  minValidLat() const;

    virtual bool traversablePoles()  const { return true; }
    virtual bool traversableDateLine()  const { return true; }

    virtual SurfaceType surfaceType() const { return Azimuthal; }

    virtual PreservationType preservationType() const { return NoPreservation; }

    virtual bool isClippedToSphere() const;

    virtual qreal clippingRadius() const;

    bool  mapCoversViewport( const ViewportParams *viewport ) const;

    virtual bool screenCoordinates( const GeoDataLineString &lineString,
                            const ViewportParams *viewport,
                            QVector<QPolygonF*> &polygons ) const;

    using AbstractProjection::screenCoordinates;

    virtual QPainterPath mapShape( const ViewportParams *viewport ) const;

    virtual GeoDataLatLonAltBox latLonAltBox( const QRect& screenRect,
                                      const ViewportParams *viewport ) const;

 protected:
    explicit AzimuthalProjection( AzimuthalProjectionPrivate* dd );

 private:
    Q_DECLARE_PRIVATE( AzimuthalProjection )
    Q_DISABLE_COPY( AzimuthalProjection )
};

}

#endif


