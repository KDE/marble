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


#ifndef GRIDMAP_H
#define GRIDMAP_H


#include <QtGui/QPen>

#include "global.h"
#include "Quaternion.h"
#include "GeoPoint.h"
#include "ScreenPolygon.h"


class ClipPainter;


class GridMap : public ScreenPolygon::Vector
{
 public:
    GridMap();
    ~GridMap();

    void createTropics( const int&, Quaternion& );
    void createEquator (const int&, Quaternion& );
    void createGrid( const int&, Quaternion& );

    void paintGridMap(ClipPainter * painter, bool );

    void resizeMap( int width, int height );

    void setPen ( const QPen & p )  { m_pen = p; }

    enum SphereDim { Longitude, Latitude };

 private:
    void createCircles( int lonNum, int latNum );
    void createCircle( double, SphereDim, double cutCoeff = 0.0 );

    const QPointF horizonPoint();

 private:
    matrix   m_planetAxisMatrix;
#ifdef FLAT_PROJ
    Quaternion m_planetAxis;
#endif
    ScreenPolygon m_polygon;

    int      m_imageWidth;
    int      m_imageHeight;
    int      m_imageRadius;

    QPointF  m_currentPoint;
    QPointF  m_lastPoint; 

    QPen     m_pen;

    //	Dealing with the horizon
    bool     m_lastVisible;
    bool     m_currentlyVisible;
	
    int      m_precision;       // Number of nodes on a quarter circle
    int      m_radius;          // The radius of the earth in pixels.
};

#endif // GRIDMAP_H
