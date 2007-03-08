#include "measuretool.h"

#include <QtCore/QDebug>
#include <QtGui/QPainter>
#include <QPoint>

#include "quaternion.h"
#include "clippainter.h"

#ifdef KDEBUILD
#include "measuretool.moc"
#endif

#include <QDebug>

MeasureTool::MeasureTool(QObject* parent) : QObject(parent) {

	m_totalDistance = 0;

	m_font_regular = QFont("Sans Serif",8, 50, false );
	
	m_fontheight = QFontMetrics(m_font_regular).height();
	m_fontascent = QFontMetrics(m_font_regular).ascent();

	m_linecolor = QColor( 255, 255, 255, 255 );

	m_useworkaround = testbug();
}

void MeasureTool::paintMeasurePoints(ClipPainter* painter, int imgrx, int imgry, int radius, Quaternion rotAxis, bool antialiasing ){
	if ( m_pMeasurePointList.isEmpty() ) return;

	if ( m_pMeasurePointList.size() > 1 ) paintTotalDistanceLabel( painter, imgrx, imgry, 10.0f );

	int imgwidth = 2 * imgrx; int imgheight = 2 * imgry;
	int x = 0; int y = 0; int lastx = 0; int lasty = 0; 

	Quaternion invRotAxis = rotAxis.inverse();
	Quaternion qpos;

	painter->setPen(QColor(Qt::white));	

	QVector<GeoPoint*>::const_iterator it;

	QPolygonF distancePath;

	for ( it= m_pMeasurePointList.constBegin(); it != m_pMeasurePointList.constEnd(); it++ ){ // STL iterators
		qpos = (*it)->getQuatPoint();
		qpos.rotateAroundAxis(invRotAxis);

		if ( qpos.v[Q_Z] > 0 ){
			x = (int)(imgrx + radius*qpos.v[Q_X]);
			y = (int)(imgry + radius*qpos.v[Q_Y]);	

			distancePath << QPointF(x,y);
		}
	}

	if (antialiasing == true) painter->setRenderHint(QPainter::Antialiasing, true);
	painter->setPen(QColor(Qt::red));	
	painter->drawPolyline(distancePath);
	if (antialiasing == true) painter->setRenderHint(QPainter::Antialiasing, false);

	for ( it= m_pMeasurePointList.constBegin(); it != m_pMeasurePointList.constEnd(); it++ ){ // STL iterators
		qpos = (*it)->getQuatPoint();
		qpos.rotateAroundAxis(invRotAxis);

		if ( qpos.v[Q_Z] > 0 ){

			x = (int)(imgrx + radius*qpos.v[Q_X]);
			y = (int)(imgry + radius*qpos.v[Q_Y]);

			// Don't process placemarks if they are outside the screen area
			if ( x >= 0 && x < imgwidth && y >= 0 && y < imgheight ){
				paintMark( painter, x, y );	
			}
		}
	}
}

void MeasureTool::paintTotalDistanceLabel( ClipPainter * painter, int imgrx, int imgry, float totalDistance ){
//	if ((totalDistance == m_totalDistance))
	QString distanceValueString;

	if ( totalDistance >= 1000.0 )
		distanceValueString = QString("%1 km").arg(totalDistance/1000.0f);		
	else	
		distanceValueString = QString("%1 m").arg(totalDistance);		

	QString distanceString = QString("Distance: %1").arg(distanceValueString);

	painter->setPen(QColor(Qt::black));
	painter->setBrush(QColor(192,192,192,192));

 	painter->drawRect(5,5, 40, 10);
	painter->drawText( 10,10, distanceString );
}

void MeasureTool::paintMark( ClipPainter* painter, int x, int y ){
	int halfsize = 5;

	painter->setPen(QColor(Qt::white));
	painter->drawLine( x - halfsize, y, x + halfsize , y );
	painter->drawLine( x, y - halfsize, x, y + halfsize );
}

bool MeasureTool::testbug(){

	QString testchar("K");
	QFont font("Sans Serif",10);

	int fontheight = QFontMetrics(font).height();
	int fontwidth = QFontMetrics(font).width(testchar);
	int fontascent = QFontMetrics(font).ascent();

	QPixmap pixmap ( fontwidth, fontheight );
	pixmap.fill(Qt::transparent);

	QPainter textpainter;
	textpainter.begin(&pixmap);
		textpainter.setPen( QColor(0,0,0,255) );
		textpainter.setFont( font );
		textpainter.drawText( 0, fontascent, testchar );
	textpainter.end();

	QImage image = pixmap.toImage();

	for (int x = 0; x < fontwidth; x++)
		for (int y = 0; y < fontheight; y++){
			if ( qAlpha( image.pixel( x,y ) ) > 0 ) return false;
		}

	return true;
}
