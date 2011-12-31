//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009      Patrick Spendrin <ps_ml@gmx.de>
//

#ifndef MARBLE_ABSTRACTPROJECTIONPRIVATE_H
#define MARBLE_ABSTRACTPROJECTIONPRIVATE_H


namespace Marble
{

class AbstractProjection;

class AbstractProjectionPrivate
{
  public:
    AbstractProjectionPrivate( AbstractProjection * _q );

    AbstractProjection * q;

    bool   m_repeatX;              // Map repeated in X direction.

    qreal  m_maxLat;
    qreal  m_minLat;

    GeoDataCoordinates findHorizon( const GeoDataCoordinates & previousCoords,
                                    const GeoDataCoordinates & currentCoords,
                                    const ViewportParams *viewport,
                                    TessellationFlags f = 0,
                                    int recursionCounter = 0 ) const;

    bool globeHidesPoint( const GeoDataCoordinates &coordinates,
                          const ViewportParams *viewport ) const;

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

    // This method tessellates a line segment in a way that the line segment
    // follows great circles. The count parameter specifies the
    // number of nodes generated for the polygon. If the
    // clampToGround flag is added the polygon contains count + 2
    // nodes as the clamped down start and end node get added.

    void tessellateLineSegment( const GeoDataCoordinates &aCoords,
                                qreal ax, qreal ay,
                                const GeoDataCoordinates &bCoords,
                                qreal bx, qreal by,
                                QPolygonF * polygon,
                                const ViewportParams *viewport,
                                TessellationFlags f = 0 ) const;

    QPolygonF processTessellation(  const GeoDataCoordinates &previousCoords,
                                    const GeoDataCoordinates &currentCoords,
                                    int count, const ViewportParams *viewport,
                                    TessellationFlags f = 0 ) const;

    bool lineStringToPolygon( const GeoDataLineString &lineString,
                              const ViewportParams *viewport,
                              QVector<QPolygonF*> &polygons ) const;

    void repeatPolygons( const ViewportParams *viewport,
                         QVector<QPolygonF *> &polygons ) const;

    void translatePolygons( const QVector<QPolygonF *> &polygons,
                            QVector<QPolygonF *> &translatedPolygons,
                            qreal xOffset ) const;
};

} // namespace Marble

#endif
