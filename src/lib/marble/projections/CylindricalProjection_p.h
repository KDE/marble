// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2009 Patrick Spendrin <ps_ml@gmx.de>
// SPDX-FileCopyrightText: 2012 Cezar Mocan <mocancezar@gmail.com>
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
    explicit CylindricalProjectionPrivate(CylindricalProjection *parent);

    // This method tessellates a line segment in a way that the line segment
    // follows great circles. The count parameter specifies the
    // number of nodes generated for the polygon. If the
    // clampToGround flag is added the polygon contains count + 2
    // nodes as the clamped down start and end node get added.

    int tessellateLineSegment(const GeoDataCoordinates &aCoords,
                              qreal ax,
                              qreal ay,
                              const GeoDataCoordinates &bCoords,
                              qreal bx,
                              qreal by,
                              QList<QPolygonF *> &polygons,
                              const ViewportParams *viewport,
                              TessellationFlags f = TessellationFlags(),
                              int mirrorCount = 0,
                              qreal repeatDistance = 0) const;

    int processTessellation(const GeoDataCoordinates &previousCoords,
                            const GeoDataCoordinates &currentCoords,
                            int count,
                            QList<QPolygonF *> &polygons,
                            const ViewportParams *viewport,
                            TessellationFlags f = TessellationFlags(),
                            int mirrorCount = 0,
                            qreal repeatDistance = 0) const;

    static int crossDateLine(const GeoDataCoordinates &aCoord,
                             const GeoDataCoordinates &bCoord,
                             qreal bx,
                             qreal by,
                             QList<QPolygonF *> &polygons,
                             int mirrorCount = 0,
                             qreal repeatDistance = 0);

    bool lineStringToPolygon(const GeoDataLineString &lineString, const ViewportParams *viewport, QList<QPolygonF *> &polygons) const;

    static void translatePolygons(const QList<QPolygonF *> &polygons, QList<QPolygonF *> &translatedPolygons, qreal xOffset);

    void repeatPolygons(const ViewportParams *viewport, QList<QPolygonF *> &polygons) const;

    qreal repeatDistance(const ViewportParams *viewport) const;

    CylindricalProjection *const q_ptr;
    Q_DECLARE_PUBLIC(CylindricalProjection)
};

} // namespace Marble

#endif
