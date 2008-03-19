//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2006-2007 Torsten Rahn <tackat@kde.org>"
// Copyright 2007      Inge Wallin  <ingwa@kde.org>"
//


#ifndef VECTORMAP_H
#define VECTORMAP_H

#include <QtGui/QPen>
#include <QtGui/QBrush>
#include <QtGui/QImage>
#include <QtGui/QPixmap>

#include "global.h"
#include "Quaternion.h"
#include "GeoDataPoint.h"
#include "ScreenPolygon.h"


class ClipPainter;
class PntMap;
class ViewportParams;

class VectorMap : public ScreenPolygon::Vector
{
 public:
    VectorMap();
    ~VectorMap();
    void createFromPntMap( const PntMap*, ViewportParams *viewport );

    void paintBase( ClipPainter *painter, ViewportParams *viewport, bool antialiasing );
    void paintMap( ClipPainter *painter, bool antialiasing );
    void drawMap( QPaintDevice *paintDevice, bool antialiasing,
		  ViewportParams *viewport );

    void resizeMap( int width, int height );

    void setPen ( const QPen & p )     { m_pen   = p; }
    void setBrush ( const QBrush & b ) { m_brush = b; }

    void setzBoundingBoxLimit ( const double zBoundingBoxLimit ) {
        m_zBoundingBoxLimit = zBoundingBoxLimit; }
    void setzPointLimit ( const double zPointLimit ) {
        m_zPointLimit = zPointLimit; }

    //	void clearNodeCount(){ m_debugNodeCount = 0; }
    //	int nodeCount(){ return m_debugNodeCount; }

 private:
    void sphericalCreateFromPntMap( const PntMap*, ViewportParams *viewport );
    void rectangularCreateFromPntMap( const PntMap*, ViewportParams *viewport );

    void createPolyLine( GeoDataPoint::Vector::ConstIterator, 
                         GeoDataPoint::Vector::ConstIterator, const int, 
			 ViewportParams *viewport );
    void sphericalCreatePolyLine( GeoDataPoint::Vector::ConstIterator, 
				  GeoDataPoint::Vector::ConstIterator, 
				  const int detail, ViewportParams *viewport );
    void rectangularCreatePolyLine( GeoDataPoint::Vector::ConstIterator, 
				    GeoDataPoint::Vector::ConstIterator, 
				    const int detail, ViewportParams *viewport );

    void sphericalPaintBase(   ClipPainter *painter, ViewportParams *viewport, bool antialiasing );
    void rectangularPaintBase( ClipPainter *painter, ViewportParams *viewport, bool antialiasing );

    void           manageCrossHorizon();
    const QPointF  horizonPoint();
    void           createArc();

    int            getDetailLevel( int radius ) const;

 private:
    GeoDataPoint::Vector  m_boundary;

    double            m_zlimit;
    double            m_plimit;
    double            m_zBoundingBoxLimit;	
    double            m_zPointLimit;	

    //	Quaternion m_invRotAxis;
    matrix            m_rotMatrix;

    //Used for flat projection
    Quaternion m_planetAxis;
    int m_lastSign;
    int m_offset;
    double m_lastLon;
    double m_lastLat;

    //	int m_debugNodeCount;

    ScreenPolygon     m_polygon;

    int               m_imgrx;
    int               m_imgry;
    int               m_imgradius;
    int               m_imgwidth;
    int               m_imgheight;

    QPointF           m_currentPoint;
    QPointF           m_lastPoint; 

    QPen              m_pen;
    QBrush            m_brush;

    // Dealing with the horizon
    bool              m_firsthorizon;
    bool              m_lastvisible;
    bool              m_currentlyvisible;
    bool              m_horizonpair;
    QPointF           m_firstHorizonPoint;
    QPointF           m_horizona;
    QPointF           m_horizonb;
	
    int               m_rlimit;

    //Needed for the flat projection
    double m_centerLat;
    double m_centerLon;
    double m_rad2Pixel;
};


#endif // VECTORMAP_H
