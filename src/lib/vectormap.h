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
#include "GeoPoint.h"
#include "ScreenPolygon.h"


class PntMap;
class ClipPainter;


class VectorMap : public ScreenPolygon::Vector
{
 public:
    VectorMap();
    ~VectorMap();
    void createFromPntMap(const PntMap*, const int&, Quaternion&);

    void paintBase(ClipPainter *, int radius, bool );
    void paintMap(ClipPainter *, bool );
    void drawMap(QPaintDevice *, bool );

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
    void createPolyLine( GeoPoint::Vector::ConstIterator, 
                         GeoPoint::Vector::ConstIterator, const int);

    void           manageCrossHorizon();
    const QPointF  horizonPoint();
    void           createArc();

    int            getDetailLevel() const;

 private:
    GeoPoint::Vector  m_boundary;

    double            m_zlimit;
    double            m_plimit;
    double            m_zBoundingBoxLimit;	
    double            m_zPointLimit;	

    //	Quaternion m_invRotAxis;
    matrix            m_rotMatrix;
#ifdef FLAT_PROJ
    Quaternion m_planetAxis;
    int m_lastSign;
    int m_offset;
    int m_lastX;
    int m_lastY;
#endif
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
	
    int               m_radius;
    int               m_rlimit;
};


#endif // VECTORMAP_H
