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
#include "ScreenPolygon.h"


class ViewportParams;
class ViewParams;               /* FIXME: Remove */
class ClipPainter;


class GridMap : public ScreenPolygon::Vector
{
 public:
    GridMap();
    ~GridMap();

    void createTropics( ViewportParams *viewport );
    void createEquator ( ViewportParams *viewport);
    void createGrid( ViewportParams *port );

    void paintGridMap(ClipPainter * painter, bool );

    void resizeMap( int width, int height );

    void setPen ( const QPen & p )  { m_pen = p; }

    enum SphereDim { Longitude, Latitude };

 private:
    void createCircles( int lonNum, int latNum, int precision,
                        ViewportParams *port );
    void sphericalCreateCircles( int lonNum, int latNum, int precision, 
                                 ViewportParams *viewport );
    void rectangularCreateCircles( int lonNum, int latNum, int precision, 
                                   ViewportParams *viewport );

    void createCircle( double val, SphereDim, 
                       int precision,
                       ViewportParams *viewport, double cutCoeff = 0.0 );
    void sphericalCreateCircle( double val, SphereDim,
                                int precision,
                                ViewportParams *viewport, double cutCoeff = 0.0 );
    void rectangularCreateCircle( double val, SphereDim,
                                  int precision,
                                  ViewportParams *viewport, double cutCoeff = 0.0 );

    int            getPrecision( ViewportParams *viewport );
    const QPointF  horizonPoint( ViewportParams *viewport )
;

 private:
    matrix   m_planetAxisMatrix;

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
};

#endif // GRIDMAP_H
