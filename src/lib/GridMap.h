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

namespace Marble
{

class ViewportParams;
class GeoPainter;

class GridMap : public ScreenPolygon::Vector
{
 public:
    GridMap();
    ~GridMap();

    void createTropics( ViewportParams *viewport );
    void createEquator ( ViewportParams *viewport);
    void createGrid( ViewportParams *port );

    void paintGridMap(GeoPainter * painter, bool );

    void setPen ( const QPen & p )  { m_pen = p; }

    enum SphereDim { Longitude, Latitude };

 private:
    void createCircles( int lonNum, int latNum, int precision,
                        ViewportParams *port );

    void createCircle( double angle, SphereDim, 
                       int precision,
                       ViewportParams *viewport, double cutCoeff = 0.0 );
    void sphericalCreateCircle( double angle, SphereDim,
                                int precision,
                                ViewportParams *viewport, double cutCoeff = 0.0 );
    void flatCreateCircle( double angle, SphereDim,
			   int precision,
			   ViewportParams *viewport, double cutCoeff = 0.0 );

    int            getPrecision( ViewportParams *viewport );
    const QPointF  horizonPoint( ViewportParams *viewport );

 private:
    matrix   m_planetAxisMatrix;

    ScreenPolygon m_polygon;

    QPointF  m_currentPoint;
    QPointF  m_lastPoint; 

    QPen     m_pen;

    //	Dealing with the horizon
    bool     m_lastVisible;
    bool     m_currentlyVisible;
};

}

#endif // GRIDMAP_H
