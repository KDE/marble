#ifndef CLIPPAINTER_H
#define CLIPPAINTER_H


#include <QtGui/QPainter>


/**
@author Torsten Rahn
*/

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
