//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2006-2007 Torsten Rahn <tackat@kde.org>
// Copyright 2007-2008 Inge Wallin  <ingwa@kde.org>
//

#ifndef MARBLE_VECTORMAP_H
#define MARBLE_VECTORMAP_H

#include <QtCore/QPointF>
#include <QtGui/QPen>
#include <QtGui/QBrush>

#include "MarbleGlobal.h"
#include "Quaternion.h"
#include "GeoDataCoordinates.h"
#include "ScreenPolygon.h"

class QPaintDevice;

namespace Marble
{

class GeoPainter;
class PntMap;
class ViewportParams;

class VectorMap
{
 public:
    VectorMap();
    ~VectorMap();
    void createFromPntMap( const PntMap*, const ViewportParams *viewport );

    /**
     * @brief Paint the background, i.e. the water.
     */
    void paintMap( GeoPainter *painter );
    void drawMap( GeoPainter *painter );

    void setzBoundingBoxLimit ( const qreal zBoundingBoxLimit ) {
        m_zBoundingBoxLimit = zBoundingBoxLimit; }
    void setzPointLimit ( const qreal zPointLimit ) {
        m_zPointLimit = zPointLimit; }

    //	void clearNodeCount(){ m_debugNodeCount = 0; }
    //	int nodeCount(){ return m_debugNodeCount; }

 private:
    void sphericalCreateFromPntMap( const PntMap*, const ViewportParams *viewport );
    void rectangularCreateFromPntMap( const PntMap*, const ViewportParams *viewport );
    void mercatorCreateFromPntMap( const PntMap*, const ViewportParams *viewport );

    void sphericalCreatePolyLine( GeoDataCoordinates::Vector::ConstIterator const &,
				  GeoDataCoordinates::Vector::ConstIterator const &,
                                  const int detail, const ViewportParams *viewport );
    void rectangularCreatePolyLine( GeoDataCoordinates::Vector::ConstIterator const &,
				    GeoDataCoordinates::Vector::ConstIterator const &,
                                    const int detail, const ViewportParams *viewport, int offset );
    void mercatorCreatePolyLine( GeoDataCoordinates::Vector::ConstIterator const &,
				 GeoDataCoordinates::Vector::ConstIterator const &,
                                 const int detail, const ViewportParams *viewport, int offset );

    QPointF  horizonPoint( const ViewportParams *viewport, const QPointF &currentPoint, int rLimit ) const;
    void           createArc( const ViewportParams *viewport, const QPointF &horizona, const QPointF &horizonb, int rLimit );

    int            getDetailLevel( int radius ) const;

 private:
    qreal            m_zBoundingBoxLimit;
    qreal            m_zPointLimit;

    ScreenPolygon::Vector m_polygons;

    //	Quaternion m_invRotAxis;
    matrix            m_rotMatrix;

    //	int m_debugNodeCount;

    ScreenPolygon     m_polygon;
};

}

#endif
