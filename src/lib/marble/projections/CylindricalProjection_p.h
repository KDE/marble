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

#ifndef MARBLE_CYLINDRICALPROJECTIONPRIVATE_H
#define MARBLE_CYLINDRICALPROJECTIONPRIVATE_H

#include "AbstractProjection_p.h"


namespace Marble
{

class CylindricalProjection;

class CylindricalProjectionPrivate : public AbstractProjectionPrivate
{
  public:
    explicit CylindricalProjectionPrivate( CylindricalProjection * parent );

    // This method tessellates a line segment in a way that the line segment
    // follows great circles. The count parameter specifies the
    // number of nodes generated for the polygon. If the
    // clampToGround flag is added the polygon contains count + 2
    // nodes as the clamped down start and end node get added.

    int tessellateLineSegment(  const GeoDataCoordinates &aCoords,
                                qreal ax, qreal ay,
                                const GeoDataCoordinates &bCoords,
                                qreal bx, qreal by,
                                QVector<QPolygonF*> &polygons,
                                const ViewportParams *viewport,
                                TessellationFlags f = 0,
                                int mirrorCount = 0,
                                qreal repeatDistance = 0 ) const;

    int processTessellation(   const GeoDataCoordinates &previousCoords,
                               const GeoDataCoordinates &currentCoords,
                               int count,
                               QVector<QPolygonF*> &polygons,
                               const ViewportParams *viewport,
                               TessellationFlags f = 0,
                               int mirrorCount = 0,
                               qreal repeatDistance = 0 ) const;

    static int crossDateLine( const GeoDataCoordinates & aCoord,
                              const GeoDataCoordinates & bCoord,
                              qreal bx,
                              qreal by,
                              QVector<QPolygonF*> &polygons,
                              int mirrorCount = 0,
                              qreal repeatDistance = 0 );

    bool lineStringToPolygon( const GeoDataLineString &lineString,
                              const ViewportParams *viewport,
                              QVector<QPolygonF*> &polygons ) const;

    static void translatePolygons( const QVector<QPolygonF *> &polygons,
                                   QVector<QPolygonF *> &translatedPolygons,
                                   qreal xOffset );

    void repeatPolygons( const ViewportParams *viewport,
                         QVector<QPolygonF *> &polygons ) const;

    qreal repeatDistance( const ViewportParams *viewport ) const;

    CylindricalProjection * const q_ptr;
    Q_DECLARE_PUBLIC( CylindricalProjection )
};

} // namespace Marble

#endif
