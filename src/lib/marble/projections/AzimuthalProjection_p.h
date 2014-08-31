//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
// Copyright 2014	   Gábor Péterffy <peterffy95@gmail.com>
//

#ifndef MARBLE_AZIMUTHALPROJECTIONPRIVATE_H
#define MARBLE_AZIMUTHALPROJECTIONPRIVATE_H

#include "AbstractProjection_p.h"


namespace Marble
{

// Maximum amount of nodes that are created automatically between actual nodes.
static const int maxTessellationNodes = 200;


class AzimuthalProjection;

class AzimuthalProjectionPrivate : public AbstractProjectionPrivate
{
public:
    explicit AzimuthalProjectionPrivate( AzimuthalProjection * parent )
        : AbstractProjectionPrivate( parent ),
          q_ptr( parent )
    {

    }

    virtual ~AzimuthalProjectionPrivate() {};

    // This method tessellates a line segment in a way that the line segment
    // follows great circles. The count parameter specifies the
    // number of nodes generated for the polygon. If the
    // clampToGround flag is added the polygon contains count + 2
    // nodes as the clamped down start and end node get added.

    void tessellateLineSegment(  const GeoDataCoordinates &aCoords,
                                qreal ax, qreal ay,
                                const GeoDataCoordinates &bCoords,
                                qreal bx, qreal by,
                                QVector<QPolygonF*> &polygons,
                                const ViewportParams *viewport,
                                TessellationFlags f = 0 ) const;

    void processTessellation(   const GeoDataCoordinates &previousCoords,
                               const GeoDataCoordinates &currentCoords,
                               int count,
                               QVector<QPolygonF*> &polygons,
                               const ViewportParams *viewport,
                               TessellationFlags f = 0 ) const;

    void crossHorizon( const GeoDataCoordinates & bCoord,
                       QVector<QPolygonF*> &polygons,
                       const ViewportParams *viewport ) const;

    virtual bool lineStringToPolygon( const GeoDataLineString &lineString,
                              const ViewportParams *viewport,
                              QVector<QPolygonF*> &polygons ) const;

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

    AzimuthalProjection * const q_ptr;

    Q_DECLARE_PUBLIC( AzimuthalProjection )
};

} // namespace Marble

#endif
