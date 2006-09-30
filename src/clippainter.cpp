#include <QDebug>
#include "clippainter.h"

ClipPainter::ClipPainter(QPaintDevice * pd, bool clip):QPainter(pd){ 
	imgwidth = pd -> width(); imgheight = pd -> height();
	currentxpos = currentypos = 0;
//	firstborder = true;
	left = 0; right = imgwidth-1;
	top = 0; bottom = imgheight-1;	

// qDebug("La");

	m_clip = true;
//	penblue.setColor(QColor( 0, 0, 255, 255));
//	pengreen.setColor(QColor( 0, 255, 0, 255));
}

void ClipPainter::drawPolygon ( const QPolygon & pa, Qt::FillRule fillRule ){

//	m_clip = false;

	if (m_clip == true){	
		drawPolyobject( pa );
		if (m_clipped.size() > 2){
//			qDebug() << m_clipped.size();
			QPainter::drawPolygon ( m_clipped , fillRule );
//			qDebug() << "done";
		}
	}
	else
		QPainter::drawPolygon ( pa , fillRule );
}

void ClipPainter::drawPolyline ( const QPolygon & pa ){

//	m_clip = false;

	if (m_clip == true){	
		drawPolyobject( pa );
		if (m_clipped.size() > 1){
//			qDebug() << m_clipped.size();
			QPainter::drawPolyline ( m_clipped );
//			qDebug() << "done";
		}
	}
	else
		QPainter::drawPolyline ( pa );
}

void ClipPainter::drawPolyobject ( const QPolygon & pa ){

/* If we think of the image borders as (infinetly long) parallels then the on-
 * and offscreen are divided into 9 sections. Each of these sections is 
 * identified by a unique keynumber (currentpos):
 *
 *	012
 *	345 <- keynumber "4" represents the onscreen section
 *	678
 */
	m_clipped.clear();

	QVector<QPoint>::const_iterator itPoint;
	const QVector<QPoint>::const_iterator itStartPoint = pa.begin();
	const QVector<QPoint>::const_iterator itEndPoint = pa.end();

	for ( itPoint = itStartPoint; itPoint != itEndPoint; itPoint++ ){

		currentPoint = (*itPoint);

// figure out the section of the current point 
		currentxpos = 1;
		if (currentPoint.x() < left) currentxpos = 0;
		if (currentPoint.x() >= right) currentxpos = 2;
							
		currentypos = 3;
		if (currentPoint.y() < top) currentypos = 0;
		if (currentPoint.y() >= bottom) currentypos = 6;

		currentpos = currentypos + currentxpos;

// initialize a few remaining variables
		if (itPoint == itStartPoint){
			lastpos = currentpos;
		}

// if the current point reaches a new section take care of clipping
		if (currentpos != lastpos){
			if (currentpos == 4 || lastpos == 4)
				m_clipped << borderPoint();
			else
				manageOffScreen();		

			lastpos = currentpos;	
		}

// if the current point is onscreen just add it to our final polygon
		if (currentpos == 4) m_clipped << currentPoint;

		lastPoint = currentPoint;
	}
}

void ClipPainter::manageOffScreen(){
// take care of adding nodes in the image corners if the iterator 
// traverses offscreen sections

	switch (currentpos){
		case 0:
			m_clipped << QPoint(left,top);
			break;
		case 1:
			if (lastpos == 3)
				m_clipped << QPoint(left,top);
			if (lastpos == 5)
				m_clipped << QPoint(right,top);
			break;
		case 2:
			m_clipped << QPoint(right,top);
			break;
		case 3:
			if (lastpos == 1)
				m_clipped << QPoint(left,top);
			if (lastpos == 7)
				m_clipped << QPoint(left,bottom);
			break;
		case 5:
			if (lastpos == 1)
				m_clipped << QPoint(right,top);
			if (lastpos == 7)
				m_clipped << QPoint(right,bottom);
			break;
		case 6:
			m_clipped << QPoint(left,bottom);
			break;
		case 7:
			if (lastpos == 3)
				m_clipped << QPoint(left,bottom);
			if (lastpos == 5)
				m_clipped << QPoint(right,bottom);
			break;
		case 8:
			m_clipped << QPoint(right,bottom);
			break;
		default:
			break;				
	}
}

const QPoint ClipPainter::borderPoint(){

//	interpolate border points (linear interpolation)
//	FIXME: take care of the corners	

	int xa = 0;
	int ya = 0;
	float m = (float)(currentPoint.y()-lastPoint.y())/(float)(currentPoint.x()-lastPoint.x());

	int offscreenpos = (currentpos == 4) ? lastpos : currentpos;

	switch (offscreenpos){
		case 0: // topleft
			xa = left;
			ya = top;
			break;
		case 1: // top
			xa = (int)(lastPoint.x() + (float)(top - lastPoint.y()) / m);
			ya = top;
			break;
		case 2: // topright
			xa = right;
			ya = top;
			break;
		case 3: // left
			xa = left;
			ya = (int)(m * (float)(left - lastPoint.x())) + lastPoint.y();
			break;
		case 5: // right
			xa = right;
			ya = (int)(m * (float)(right - lastPoint.x())) + lastPoint.y();
			break;
		case 6: // bottomleft
			xa = left;
			ya = bottom;
			break;
		case 7: // bottom
			xa = (int)(lastPoint.x() + (float)(bottom - lastPoint.y()) / m);
			ya = bottom;
			break;
		case 8: // bottomright
			xa = right;
			ya = bottom;
			break;
		default:
			break;			
	}

	return QPoint(xa, ya); 	
}
