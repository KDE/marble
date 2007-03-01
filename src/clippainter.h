#ifndef CLIPPAINTER_H
#define CLIPPAINTER_H

#include <QPainter>

/**
@author Torsten Rahn
*/

class ClipPainter : public QPainter {
public:
	ClipPainter();
	ClipPainter(QPaintDevice*, bool);
	~ClipPainter(){};
	void drawPolygon ( const QPolygonF &, Qt::FillRule fillRule = Qt::OddEvenFill );
	void drawPolyline ( const QPolygonF & );
private:
	void drawPolyobject ( const QPolygonF & );

	bool m_clip;
	float left, right, top, bottom;
	int imgwidth, imgheight;
	int currentpos, currentxpos, currentypos;
	int lastpos;

	void manageOffScreen();
	const QPointF borderPoint();

	QPointF m_lastBorderPoint;
	QPointF m_currentPoint, m_lastPoint; 
	QPolygonF m_clipped;

};

#endif // CLIPPAINTER_H
