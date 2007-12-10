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

//
// MeasureTool enables Marble to set and display measure points
//


#ifndef MEASURETOOL_H
#define MEASURETOOL_H


#include <QtCore/QVector>
#include <QtGui/QFont>

#include "GeoDataPoint.h"
#include "global.h"
#include "ClipPainter.h"


class MeasureTool : public QObject
{
    Q_OBJECT

 public:
    MeasureTool( QObject *parent = 0 );

    void  paintMeasurePoints( ClipPainter*, int, int, int, Quaternion, bool, Projection );
    void  sphericalPaintMeasurePoints ( ClipPainter*, int, int, int, Quaternion, bool );
    void  rectangularPaintMeasurePoints ( ClipPainter*, int, int, int, Quaternion, bool );

    void  setLineColor( QColor linecolor ) { m_linecolor = linecolor; }
    void  paintTotalDistanceLabel( ClipPainter*, int, int, double );

 public Q_SLOTS:
    void addMeasurePoint( double lon, double lat ) {
        m_pMeasurePointList << new GeoDataPoint( lon, lat );
    }
    void removeMeasurePoints() {
        m_pMeasurePointList.clear();
    }

 protected:
    bool  testbug(); 
    void  paintMark( ClipPainter* painter, int x, int y );
    void  drawDistancePath( ClipPainter* painter, Quaternion, Quaternion, 
                            int imgrx, int imgry, int radius, 
                            bool antialiasing, Projection );

    void  sphericalDrawDistancePath( ClipPainter* painter, Quaternion, Quaternion, 
                            int imgrx, int imgry, int radius, 
                            bool antialiasing );

    void  rectangularDrawDistancePath( ClipPainter* painter, Quaternion, Quaternion, 
                            int imgrx, int imgry, int radius, 
                            bool antialiasing );

    double  m_totalDistance;

    QFont   m_font_regular;
    int     m_fontheight;
    int     m_fontascent;

    QColor  m_linecolor;

    QVector<GeoDataPoint*>  m_pMeasurePointList;

    QPen    m_pen;

    QPixmap m_empty;

    bool    m_useworkaround;
    //Needed for the flat projection
    double m_centerLat;
    double m_centerLon;
    double m_xyFactor;
    int m_radius;
};


#endif // MEASURETOOL_H
