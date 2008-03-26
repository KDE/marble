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

class ViewportParams;

class MeasureTool : public QObject
{
    Q_OBJECT

 public:
    MeasureTool( QObject *parent = 0 );

    void  paint( ClipPainter *painter, ViewportParams *viewport,
                 bool antialiasing );

 public Q_SLOTS:
    void  addMeasurePoint( double lon, double lat ) {
        m_pMeasurePointList << new GeoDataPoint( lon, lat );
    }
    void  removeMeasurePoints() {
        m_pMeasurePointList.clear();
    }

 private:
    void  drawDistancePoints( ClipPainter *painter, ViewportParams *viewport );
    void  drawMark( ClipPainter* painter, ViewportParams *viewport,
                    int x, int y );
    void  drawDistancePath( ClipPainter* painter,
                            Quaternion, Quaternion,
                            ViewportParams *viewport );
    void  drawTotalDistanceLabel( ClipPainter *painter,
                                  double totalDistance );

    bool  testBug(); 

 private:
    // The list of points in the distance path.
    QVector<GeoDataPoint*>  m_pMeasurePointList;

    QFont   m_font_regular;
    int     m_fontheight;
    int     m_fontascent;

    QPen    m_pen;

    bool    m_useworkaround;

    //Needed for the flat projection
    QRectF m_visibleArea;

    double m_centerLat;
    double m_centerLon;
    int m_radius;
};

#endif // MEASURETOOL_H
