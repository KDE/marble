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

#include "global.h"
#include "Quaternion.h"
#include "GeoDataCoordinates.h"
#include "ScreenPolygon.h"

class QPaintDevice;

namespace Marble
{

class GeoPainter;
class PntMap;
class ViewportParams;

class VectorMap : public ScreenPolygon::Vector
{
 public:
    VectorMap();
    ~VectorMap();
    void createFromPntMap( const PntMap*, ViewportParams *viewport );

    /**
     * @brief Paint the background, i.e. the water.
     */
    void paintBase( GeoPainter *painter, ViewportParams *viewport, bool antialiasing );
    void paintMap( GeoPainter *painter, bool antialiasing );
    void drawMap( QPaintDevice *paintDevice, bool antialiasing,
		  ViewportParams *viewport, MapQuality mapQuality );

    void setPen ( const QPen & p )     { m_pen   = p; }
    void setBrush ( const QBrush & b ) { m_brush = b; }

    void setzBoundingBoxLimit ( const qreal zBoundingBoxLimit ) {
        m_zBoundingBoxLimit = zBoundingBoxLimit; }
    void setzPointLimit ( const qreal zPointLimit ) {
        m_zPointLimit = zPointLimit; }

    //	void clearNodeCount(){ m_debugNodeCount = 0; }
    //	int nodeCount(){ return m_debugNodeCount; }

 private:
    void sphericalCreateFromPntMap( const PntMap*, ViewportParams *viewport );
    void rectangularCreateFromPntMap( const PntMap*, ViewportParams *viewport );
    void mercatorCreateFromPntMap( const PntMap*, ViewportParams *viewport );

    void createPolyLine( GeoDataCoordinates::Vector::ConstIterator const &,
                         GeoDataCoordinates::Vector::ConstIterator const &, const int,
			 ViewportParams *viewport );
    void sphericalCreatePolyLine( GeoDataCoordinates::Vector::ConstIterator const &,
				  GeoDataCoordinates::Vector::ConstIterator const &,
				  const int detail, ViewportParams *viewport );
    void rectangularCreatePolyLine( GeoDataCoordinates::Vector::ConstIterator const &,
				    GeoDataCoordinates::Vector::ConstIterator const &,
				    const int detail, ViewportParams *viewport );
    void mercatorCreatePolyLine( GeoDataCoordinates::Vector::ConstIterator const &,
				 GeoDataCoordinates::Vector::ConstIterator const &,
				 const int detail, ViewportParams *viewport );

    void           manageCrossHorizon(ViewportParams *viewport);
    const QPointF  horizonPoint(ViewportParams *viewport);
    void           createArc(ViewportParams *viewport);

    int            getDetailLevel( int radius ) const;

 private:
    qreal            m_zlimit;
    qreal            m_plimit;
    qreal            m_zBoundingBoxLimit;	
    qreal            m_zPointLimit;	

    //	Quaternion m_invRotAxis;
    matrix            m_rotMatrix;

    //	int m_debugNodeCount;

    ScreenPolygon     m_polygon;

    QPointF           m_currentPoint;
    QPointF           m_lastPoint; 

    QPen              m_pen;
    QBrush            m_brush;

    // Dealing with the horizon for spherical projection.
    bool              m_firsthorizon;
    bool              m_lastvisible;
    bool              m_currentlyvisible;
    bool              m_horizonpair;
    QPointF           m_firstHorizonPoint;
    QPointF           m_horizona;
    QPointF           m_horizonb;
	
    int               m_rlimit;

    // Needed for repetition in the X direction for flat projection
    int         m_lastSign;
    int         m_offset;
    qreal      m_lastLon;
    qreal      m_lastLat;
};

}

#endif
