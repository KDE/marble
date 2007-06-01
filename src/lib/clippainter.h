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


#ifndef CLIPPAINTER_H
#define CLIPPAINTER_H


#include <QtGui/QPainter>



class ClipPainter : public QPainter 
{
 public:
    ClipPainter();
    ClipPainter(QPaintDevice*, bool);
    ~ClipPainter(){};

    void drawPolygon( const QPolygonF &, 
                      Qt::FillRule fillRule = Qt::OddEvenFill );
    void drawPolyline( const QPolygonF & );

    //	void clearNodeCount(){ m_debugNodeCount = 0; }
    //	int nodeCount(){ return m_debugNodeCount; }

 private:
    void drawPolyobject ( const QPolygonF & );

    void manageOffScreen();
    const QPointF borderPoint();

 private:
    bool   m_clip;

    float  left;
    float  right;
    float  top;
    float  bottom;

    int    imgwidth;
    int    imgheight;

    int    currentpos;
    int    currentxpos;
    int    currentypos;
    int    lastpos;

    //	int m_debugNodeCount;

    QPointF    m_lastBorderPoint;
    QPointF    m_currentPoint;
    QPointF    m_lastPoint; 

    QPolygonF  m_clipped;
};

#endif // CLIPPAINTER_H
