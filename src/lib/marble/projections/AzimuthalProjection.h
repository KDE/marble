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

    ~AzimuthalProjection() override;

    bool repeatableX() const override { return false; }
    qreal  maxValidLat() const override;
    qreal  minValidLat() const override;

    bool traversablePoles()  const override { return true; }
    bool traversableDateLine()  const override { return true; }

    SurfaceType surfaceType() const override { return Azimuthal; }

    PreservationType preservationType() const override { return NoPreservation; }

    bool isClippedToSphere() const override;

    qreal clippingRadius() const override;

    bool  mapCoversViewport( const ViewportParams *viewport ) const override;

    bool screenCoordinates( const GeoDataLineString &lineString,
                            const ViewportParams *viewport,
                            QVector<QPolygonF*> &polygons ) const override;

    using AbstractProjection::screenCoordinates;

    QPainterPath mapShape( const ViewportParams *viewport ) const override;

    GeoDataLatLonAltBox latLonAltBox( const QRect& screenRect,
                                      const ViewportParams *viewport ) const override;

 protected:
    explicit AzimuthalProjection( AzimuthalProjectionPrivate* dd );

 private:
    Q_DECLARE_PRIVATE( AzimuthalProjection )
    Q_DISABLE_COPY( AzimuthalProjection )
};

}

#endif


