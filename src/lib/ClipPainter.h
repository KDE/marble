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

namespace Marble
{

// The reason for this class is a terrible bug in some versions of the
// X Server.  Suppose the widget size is, say, 1000 x 1000 and we have
// a high zoom so that we want to draw a vector from (-100000,
// -100000) to (100000, 100000).  Then the X server will create a
// bitmap that is at least 100000 x 100000 and in the process eat all
// available memory.
//
// So we introduce the ClipPainter that clips all vectors to only the
// part of them that are actually shown.
//

class ClipPainter : public QPainter 
{
 public:
    ClipPainter();
    ClipPainter(QPaintDevice*, bool);

    void setClipping( bool enable );
    bool isClipping() const;

    void drawPolygon( const QPolygonF &, 
                      Qt::FillRule fillRule = Qt::OddEvenFill );
    void drawPolyline( const QPolygonF & );

    //	void clearNodeCount(){ m_debugNodeCount = 0; }
    //	int nodeCount(){ return m_debugNodeCount; }

 private:
    // This function does the actual clipping.
    void clipPolyObject ( const QPolygonF & );

    void manageOffScreen();
    const QPointF borderPoint();

 private:
    // true if clipping is on.
    bool    m_doClip;

    // The limits
    qreal  m_left;
    qreal  m_right;
    qreal  m_top;
    qreal  m_bottom;

    // Size of the image
    int     m_imgWidth;
    int     m_imgHeight;

    // Used in the paint process of vectors..
    int     m_currentSector;
    int     m_currentXSector;
    int     m_currentYSector;
    int     m_lastSector;

    //	int m_debugNodeCount;

    QPointF    m_lastBorderPoint;
    QPointF    m_currentPoint;
    QPointF    m_lastPoint; 

    // The resulting object from the clipping operation
    QPolygonF  m_clippedObject;
};

}

#endif // CLIPPAINTER_H
