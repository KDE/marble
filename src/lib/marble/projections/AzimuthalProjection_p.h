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

    // In order to ensure proper performance the current algorithm
    // determines whether the linestring disappears
    // behind the horizon by evaluating the actual node coordinates of the
    // linestring.
    // However in some cases only a tessellated portion of the linestring
    // disappears behind the globe. In this case non-closed linestrings
    // can still be cut into separate polygons without problems.
    // But for linearrings the horizon detection at this stage happens too
    // late already to be taken into account for rendering.
    // The allowLatePolygonCut parameter allows to split at least
    // non-closed linestrings properly at this point.

    void tessellateLineSegment(  const GeoDataCoordinates &aCoords,
                                qreal ax, qreal ay,
                                const GeoDataCoordinates &bCoords,
                                qreal bx, qreal by,
                                QVector<QPolygonF*> &polygons,
                                const ViewportParams *viewport,
                                TessellationFlags f = 0,
                                bool allowLatePolygonCut = false ) const;

    void processTessellation(   const GeoDataCoordinates &previousCoords,
                               const GeoDataCoordinates &currentCoords,
                               int count,
                               QVector<QPolygonF*> &polygons,
                               const ViewportParams *viewport,
                               TessellationFlags f = 0,
                               bool allowLatePolygonCut = false ) const;

    void crossHorizon( const GeoDataCoordinates & bCoord,
                       QVector<QPolygonF*> &polygons,
                       const ViewportParams *viewport,
                       bool allowLatePolygonCut = false
                     ) const;

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
                                    TessellationFlags f = 0) const;

    GeoDataCoordinates doFindHorizon(const GeoDataCoordinates & previousCoords,
                                     const GeoDataCoordinates & currentCoords,
                                     const ViewportParams *viewport,
                                     TessellationFlags f,
                                     bool currentHide,
                                     int recursionCounter) const;

    bool globeHidesPoint( const GeoDataCoordinates &coordinates,
                          const ViewportParams *viewport ) const;

    AzimuthalProjection * const q_ptr;

    Q_DECLARE_PUBLIC( AzimuthalProjection )
};

} // namespace Marble

#endif
