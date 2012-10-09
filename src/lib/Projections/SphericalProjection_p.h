//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009      Patrick Spendrin <ps_ml@gmx.de>
// Copyright 2012	   Cezar Mocan <mocancezar@gmail.com>
//

#ifndef MARBLE_SPHERICALPROJECTIONPRIVATE_H
#define MARBLE_SPHERICALPROJECTIONPRIVATE_H

#include "AbstractProjection_p.h"


namespace Marble
{

class SphericalProjection;

class SphericalProjectionPrivate : public AbstractProjectionPrivate
{

  public:
    SphericalProjectionPrivate( SphericalProjection * parent );

    virtual bool lineStringToPolygon( const GeoDataLineString &lineString,
                              const ViewportParams *viewport,
                              QVector<QPolygonF*> &polygons ) const;

    void manageHorizonCrossing( bool globeHidesPoint,
                                const GeoDataCoordinates& horizonCoords,
                                bool& horizonPair,
                                GeoDataCoordinates& horizonDisappearCoords,
                                bool& horizonOrphan,
                                GeoDataCoordinates& horizonOrphanCoords ) const;

    void horizonToPolygon( const ViewportParams *viewport,
                           const GeoDataCoordinates & disappearCoords,
                           const GeoDataCoordinates & reappearCoords,
                           QPolygonF* ) const;

    GeoDataCoordinates findHorizon( const GeoDataCoordinates & previousCoords,
                                    const GeoDataCoordinates & currentCoords,
                                    const ViewportParams *viewport,
                                    TessellationFlags f = 0,
                                    int recursionCounter = 0 ) const;

    bool globeHidesPoint( const GeoDataCoordinates &coordinates,
                          const ViewportParams *viewport ) const;

    Q_DECLARE_PUBLIC( SphericalProjection )
};

} // namespace Marble

#endif
