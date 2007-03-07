#include <QtCore/QDebug>
#include "clippainter.h"

ClipPainter::ClipPainter(QPaintDevice * pd, bool clip):QPainter(pd){ 
	imgwidth = pd -> width(); imgheight = pd -> height();
	currentxpos = currentypos = 0;

	left = -1.0f; right = imgwidth;
	top = -1.0f; bottom = imgheight;	


	m_clip = clip;
//	penblue.setColor(QColor( 0, 0, 255, 255));
//	pengreen.setColor(QColor( 0, 255, 0, 255));
}

void ClipPainter::drawPolygon ( const QPolygonF & pa, Qt::FillRule fillRule ){

	if (m_clip == true){	
		drawPolyobject( pa );
		if (m_clipped.size() > 2){
//			qDebug() << "Size: " << m_clipped.size();
			QPainter::drawPolygon ( m_clipped , fillRule );
//			qDebug() << "done";
		}
	}
	else
		QPainter::drawPolygon ( pa , fillRule );
}

void ClipPainter::drawPolyline ( const QPolygonF & pa ){

	if (m_clip == true){	
		drawPolyobject( pa );
		if (m_clipped.size() > 1){
//			qDebug() << "Size: " << m_clipped.size();
			QPainter::drawPolyline ( m_clipped );
//			qDebug() << "done";
		}
	}
	else
		QPainter::drawPolyline ( pa );
}

void ClipPainter::drawPolyobject ( const QPolygonF & pa ){

/* If we think of the image borders as (infinetly long) parallels then the on-
 * and offscreen are divided into 9 sections. Each of these sections is 
 * identified by a unique keynumber (currentpos):
 *
 *	012
 *	345 <- keynumber "4" represents the onscreen section
 *	678
 */
	m_clipped.clear();

	QVector<QPointF>::const_iterator itPoint;
	const QVector<QPointF>::const_iterator itStartPoint = pa.begin();
	const QVector<QPointF>::const_iterator itEndPoint = pa.end();

	for ( itPoint = itStartPoint; itPoint != itEndPoint; itPoint++ ){

		m_currentPoint = (*itPoint);

// figure out the section of the current point 
		currentxpos = 1;
		if (m_currentPoint.x() < left) currentxpos = 0;
		if (m_currentPoint.x() > right) currentxpos = 2;
							
		currentypos = 3;
		if (m_currentPoint.y() < top) currentypos = 0;
		if (m_currentPoint.y() > bottom) currentypos = 6;

		currentpos = currentypos + currentxpos;

// initialize a few remaining variables
		if (itPoint == itStartPoint){
			lastpos = currentpos;
		}

// if the current point reaches a new section take care of clipping
		if (currentpos != lastpos){
			if (currentpos == 4 || lastpos == 4){
				m_lastBorderPoint = borderPoint();
				m_clipped << m_lastBorderPoint;
			}
			else
				manageOffScreen();		

			lastpos = currentpos;	
		}

// if the current point is onscreen just add it to our final polygon
		if (currentpos == 4) m_clipped << m_currentPoint;

		m_lastPoint = m_currentPoint;
	}
}

void ClipPainter::manageOffScreen(){
// take care of adding nodes in the image corners if the iterator 
// traverses offscreen sections

//	FIXME:	- bugs related to vertical and horizontal lines in corners  
//		- borderpoint order

	float xa = 0;
	float ya = 0;

	// Calculating the slope
	float m = (m_currentPoint.y()-m_lastPoint.y())/(m_currentPoint.x()-m_lastPoint.x());

	switch (currentpos){
		case 0:
			m_clipped << QPointF(left,top);
			break;
		case 1:
			if (lastpos == 3){ // case checked
				xa = m_lastPoint.x() + (top - m_lastPoint.y()) / m;
				ya = m * (left - m_lastPoint.x()) + m_lastPoint.y();
				if ( xa < left && ya < top )
					m_clipped << QPointF(left,top);
				else {
					if ( m_lastBorderPoint.x() == left )
						m_clipped << QPointF(left,ya) << QPointF(xa,top);
					else
						m_clipped << QPointF(xa,top) << QPointF(left,ya);
				}
			}
			if (lastpos == 5){ // case checked
				xa = m_lastPoint.x() + (top - m_lastPoint.y()) / m;
				ya = m * (right - m_lastPoint.x()) + m_lastPoint.y();
				if ( xa > right && ya < top )
					m_clipped << QPointF(right,top);
				else{
					if ( m_lastBorderPoint.x() == right )
						m_clipped << QPointF(right,ya) << QPointF(xa,top);
					else
						m_clipped << QPointF(xa,top) << QPointF(right,ya);
				}
			}
			break;
		case 2:
			m_clipped << QPointF(right,top);
			break;
		case 3:
			if (lastpos == 1){
				xa = m_lastPoint.x() + (top - m_lastPoint.y()) / m;
				ya = m * (left - m_lastPoint.x()) + m_lastPoint.y();
				if ( xa < left && ya < top )
					m_clipped << QPointF(left,top);
				else{
					if ( m_lastBorderPoint.x() == left )
						m_clipped << QPointF(left,ya) << QPointF(xa,top);
					else
						m_clipped << QPointF(xa,top) << QPointF(left,ya);
				}
			}
			if (lastpos == 7){
				xa = m_lastPoint.x() + (bottom - m_lastPoint.y()) / m;
				ya = m * (left - m_lastPoint.x()) + m_lastPoint.y();
				if ( xa < left && ya > bottom )
					m_clipped << QPointF(left,bottom);
				else{
					if ( m_lastBorderPoint.x() == left )
						m_clipped << QPointF(left,ya) << QPointF(xa,bottom);
					else
						m_clipped << QPointF(xa,bottom) << QPointF(left,ya);
				}
			}
				
			break;
		case 5:
			if (lastpos == 1){ // case checked
				xa = m_lastPoint.x() + (top - m_lastPoint.y()) / m;
				ya = m * (right - m_lastPoint.x()) + m_lastPoint.y();
				if ( xa > right && ya < top )
					m_clipped << QPointF(right,top);
				else{
					if ( m_lastBorderPoint.x() == right )
						m_clipped << QPointF(right,ya) << QPointF(xa,top);
					else
						m_clipped << QPointF(xa,top) << QPointF(right,ya);
				}
			}
			if (lastpos == 7){
				xa = m_lastPoint.x() + (bottom - m_lastPoint.y()) / m;
				ya = m * (right - m_lastPoint.x()) + m_lastPoint.y();
				if ( xa > right && ya > bottom )
					m_clipped << QPointF(right,bottom);
				else{
					if ( m_lastBorderPoint.x() == right )
						m_clipped << QPointF(right,ya) << QPointF(xa,bottom);
					else
						m_clipped << QPointF(xa,bottom) << QPointF(right,ya);
				}
			}
			break;
		case 6:
			m_clipped << QPointF(left,bottom);
			break;
		case 7:
			if (lastpos == 3){
				xa = m_lastPoint.x() + (bottom - m_lastPoint.y()) / m;
				ya = m * (left - m_lastPoint.x()) + m_lastPoint.y();
				if ( xa < left && ya > bottom )
					m_clipped << QPointF(left,bottom);
				else{
					if ( m_lastBorderPoint.x() == left )
						m_clipped << QPointF(left,ya) << QPointF(xa,bottom);
					else
						m_clipped << QPointF(xa,bottom) << QPointF(left,ya);
				}
			}
			if (lastpos == 5){
				xa = m_lastPoint.x() + (bottom - m_lastPoint.y()) / m;
				ya = m * (right - m_lastPoint.x()) + m_lastPoint.y();
				if ( xa > right && ya > bottom )
					m_clipped << QPointF(right,bottom);
				else{
					if ( m_lastBorderPoint.x() == right )
						m_clipped << QPointF(right,ya) << QPointF(xa,bottom);
					else
						m_clipped << QPointF(xa,bottom) << QPointF(right,ya);
				}
			}
			break;
		case 8:
			m_clipped << QPointF(right,bottom);
			break;
		default:
			break;				
	}
}

const QPointF ClipPainter::borderPoint(){

//	interpolate border points (linear interpolation)

	float xa = 0;
	float ya = 0;

	// Calculating the slope
	float m = (m_currentPoint.y()-m_lastPoint.y())/(m_currentPoint.x()-m_lastPoint.x());

	int offscreenpos = (currentpos == 4) ? lastpos : currentpos;

	// "Rise over run" for all possible situations 
	switch (offscreenpos){
		case 0: // topleft
			xa = m_lastPoint.x() + (top - m_lastPoint.y()) / m;
			ya = m * (left - m_lastPoint.x()) + m_lastPoint.y();
			xa = ( xa < left ) ? left : xa; 
			ya = ( ya < top ) ? top : ya; 
			break;
		case 1: // top
			xa = m_lastPoint.x() + (top - m_lastPoint.y()) / m;
			ya = top;
			break;
		case 2: // topright
			xa = m_lastPoint.x() + (top - m_lastPoint.y()) / m;
			ya = m * (right - m_lastPoint.x()) + m_lastPoint.y();
			xa = ( xa > right ) ? right : xa; 
			ya = ( ya < top ) ? top : ya; 
			break;
		case 3: // left
			xa = left;
			ya = m * (left - m_lastPoint.x()) + m_lastPoint.y();
			break;
		case 5: // right
			xa = right;
			ya = m * (right - m_lastPoint.x()) + m_lastPoint.y();
			break;
		case 6: // bottomleft
			xa = m_lastPoint.x() + (bottom - m_lastPoint.y()) / m;
			ya = m * (left - m_lastPoint.x()) + m_lastPoint.y();
			xa = ( xa < left ) ? left : xa; 
			ya = ( ya > bottom ) ? bottom : ya; 
			break;
		case 7: // bottom
			xa = m_lastPoint.x() + (bottom - m_lastPoint.y()) / m;
			ya = bottom;
			break;
		case 8: // bottomright
			xa = m_lastPoint.x() + (bottom - m_lastPoint.y()) / m;
			ya = m * (right - m_lastPoint.x()) + m_lastPoint.y();
			xa = ( xa > right ) ? right : xa; 
			ya = ( ya > bottom ) ? bottom : ya; 
			break;
		default:
			break;			
	}

	return QPointF(xa, ya); 	
}
