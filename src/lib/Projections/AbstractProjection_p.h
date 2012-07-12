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
    AbstractProjectionPrivate( AbstractProjection * parent );

    virtual ~AbstractProjectionPrivate() { };


    bool   m_repeatX;              // Map repeated in X direction.

    qreal  m_maxLat;
    qreal  m_minLat;

    mutable int m_IDLCrossed;               // needed for crossDateLine method. Returns 0 if IDL was not crossed,
                                    // otherwise -1 or 1, according to the direction of crossing. 

    // This method tessellates a line segment in a way that the line segment
    // follows great circles. The count parameter specifies the
    // number of nodes generated for the polygon. If the
    // clampToGround flag is added the polygon contains count + 2
    // nodes as the clamped down start and end node get added.

    void tessellateLineSegment( const GeoDataCoordinates &aCoords,
                                qreal ax, qreal ay,
                                const GeoDataCoordinates &bCoords,
                                qreal bx, qreal by,
                                QVector<QPolygonF*> &polygons,
                                const ViewportParams *viewport,
                                const GeoDataLineString *lineString,
                                TessellationFlags f = 0 ) const;

    void processTessellation(  const GeoDataCoordinates &previousCoords,
                               const GeoDataCoordinates &currentCoords,
                               int count,
                               QVector<QPolygonF*> &polygons,
                               const ViewportParams *viewport,
                               const GeoDataLineString *lineString,
                               TessellationFlags f = 0 ) const;

    qreal mirrorPoint( const ViewportParams *viewport ) const;


    void translatePolygons( const QVector<QPolygonF *> &polygons,
                            QVector<QPolygonF *> &translatedPolygons,
                            qreal xOffset ) const;

    void crossDateLine( const GeoDataCoordinates & aCoord,
                        const GeoDataCoordinates & bCoord,
                        QVector<QPolygonF*> &polygons,
                        const ViewportParams *viewport,
                        const GeoDataLineString *lineString ) const;

    AbstractProjection * const q_ptr;
    Q_DECLARE_PUBLIC( AbstractProjection )
};

} // namespace Marble

#endif
