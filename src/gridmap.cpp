#include "gridmap.h"

#include <QtGui/QColor>
#include <QtCore/QVector>
#include <QtCore/QTime>
#include <QtCore/QDebug>

#include <cmath>
#include <stdlib.h>

#include "clippainter.h"

const float PI_HALF = M_PI / 2;
const float rad2int = 21600.0 / M_PI;
const float axialtilt = M_PI / 180.0f * ( 23.0f +  26.0f / 60.0f + 21.0f / 3600.0f );

GridMap::GridMap(){

	imgrx = 0; imgry = 0; imgradius = 0;
	imgwidth = 0; imgheight = 0;

//	Initialising booleans for horizoncrossing

	lastvisible = false;
	currentlyvisible = false;

	m_radius = 0; m_rlimit = 0;

	m_pen = QPen(QColor( 255, 255, 255, 128));
	m_precision = 10;
}

GridMap::~GridMap(){
}

void GridMap::createTropics(const int& radius, Quaternion& rotAxis){
	clear();
	m_radius = radius-1;
	rotAxis.inverse().toMatrix( m_rotMatrix );
	
	if ( m_radius >  800 ){
		createCircle( PI_HALF - axialtilt , Latitude );
		createCircle( axialtilt - PI_HALF , Latitude );
		createCircle( axialtilt , Latitude );
		createCircle( -axialtilt , Latitude );
	}
}

void GridMap::createGrid(const int& radius, Quaternion& rotAxis){
	clear();
	m_radius = radius-1;
	rotAxis.inverse().toMatrix( m_rotMatrix );

//	FIXME:	- Higher precision after optimization 
//		  ( will keep grid lines from vanishing at high zoom levels ) 

//	if ( m_radius > 6400 ) { m_precision = 30; createCircles( 64, 48 ); return; } else 
	if ( m_radius > 3200 ) { m_precision = 40; createCircles( 32, 24 ); return; }	
	else if ( m_radius > 1600 ) { m_precision = 30; createCircles( 16, 12 ); return; }	
	else if ( m_radius >  700 ) { m_precision = 30; createCircles( 8, 6 ); return; }	
	else if ( m_radius >  400 ) { m_precision = 20; createCircles( 4, 3 ); return; }	
	else if ( m_radius >  100 ) { m_precision = 10; createCircles( 2, 3 ); return; }	

	createCircles( 2, 1 );	
}

void GridMap::createCircles( const int lngNum, const int latNum ){

//	latNum: number of latitude circles between lat = 0 deg and lat < 90 deg
//	lngNum: number of longitude circles between lng = 0 deg and lng < 90 deg

	if ( latNum != 0 ){

		createCircle( 0.0f , Latitude );	// equator

		for ( int i = 1; i < latNum; ++i ){
			createCircle( + (float)(i) * PI_HALF / (float)(latNum)  , Latitude );
			createCircle( - (float)(i) * PI_HALF / (float)(latNum)  , Latitude );
		} 

	} 

	if ( lngNum == 0 ) return;

	createCircle( + 0 , Longitude );
	createCircle( + PI_HALF , Longitude );	

	for ( int i = 1; i < lngNum; ++i ){
		float cutoff = PI_HALF / (float)(latNum);
		createCircle( + (float)(i) * PI_HALF / (float)(lngNum) , Longitude, cutoff );
		createCircle( + (float)(i) * PI_HALF / (float)(lngNum) + PI_HALF , Longitude, cutoff );	
	}

}

void GridMap::createCircle( float val, SphereDim dim, float cutoff ){

//	cutoff: the amount of each quarter circle that is cut off at the pole in rad

	const float cutcoeff = 1 - cutoff / PI_HALF;
	const float quartSteps = (float) m_precision;	// curve precision

	float coeff = 1.0f;
	float offset = 0.0f;

	for ( int i = 0; i < 4; i++ ){

		m_polygon.clear();

		if ( i > 1 ) coeff = - 1.0f;
		offset = ( i % 2 ) ? 1.0f : 0.0f;

		const int steps = (int) ( cutcoeff * quartSteps );

		for ( int j = 0; j < steps + 1; j++ ){

			float itval = (j != steps) ? (float)(j)/quartSteps : cutcoeff;
			float dimVal = coeff * ( PI_HALF * fabs( offset - itval ) + offset * PI_HALF );

			float lat = ( dim == Latitude ) ? val : dimVal;
			float lng = ( dim == Longitude ) ? val : dimVal;

			GeoPoint geoit( lng, -lat );
			Quaternion qpos = geoit.quaternion();
			qpos.rotateAroundAxis(m_rotMatrix);

			currentPoint = QPointF((float)(imgrx+ m_radius*qpos.v[Q_X])+1,(float)(imgry+ m_radius*qpos.v[Q_Y])+1);
//			qDebug() << "Radius: " << m_radius << "QPointF(" << (float)(imgrx+ m_radius*qpos.v[Q_X])+1 << ", " << (float)(imgry+ m_radius*qpos.v[Q_Y])+1 << ")";			
//	Take care of horizon crossings if horizon is visible
			lastvisible = currentlyvisible;

//	Take care of horizon crossings if horizon is visible
			currentlyvisible = (qpos.v[Q_Z] >= 0) ? true : false;

			if ( j == 0 ) initCrossHorizon();

			if (currentlyvisible != lastvisible) {
				m_polygon << horizonPoint();

				if (m_polygon.size() >= 2) {
					append(m_polygon);
				}

				m_polygon.clear();
				if ( lastvisible == true ) break;
			}

//	Take care of screencrossing crossings if horizon is visible
//	Filter Points which aren't on the visible Hemisphere
			if ( currentlyvisible == true && currentPoint != lastPoint ){ // most recent addition: currentPoint != lastPoint
//			qDebug("accepted");
				m_polygon << currentPoint;
			}

			lastPoint = currentPoint;
		}

		if (m_polygon.size() >= 2) {
			append(m_polygon);
		}
	}
}

void GridMap::paintGridMap(ClipPainter * painter, bool antialiasing){

	if ( size() == 0 ) return;

	if (antialiasing == true) painter->setRenderHint(QPainter::Antialiasing, true);

	painter->setPen(m_pen);

	ScreenPolygon::Vector::const_iterator itEndPolygon = end();

	for ( ScreenPolygon::Vector::const_iterator itPolygon=begin(); itPolygon != itEndPolygon; ++itPolygon )
        {
		painter->drawPolyline(*itPolygon);
        }

	if (antialiasing == true) painter->setRenderHint(QPainter::Antialiasing, false);
}


void GridMap::initCrossHorizon(){
	lastvisible = currentlyvisible;
	// Initially lastPoint MUST NOT equal currentPoint
	lastPoint = QPointF( currentPoint.x() + 1.0f, currentPoint.y() + 1.0f );
}

const QPointF GridMap::horizonPoint(){
//	qDebug("Interpolating");
	float xa = 0;
	float ya = 0;

	xa = currentPoint.x() - (imgrx +1) ;
// move the currentPoint along the y-axis to match the horizon
	float radicant = (float)(m_radius +1) * (float)( m_radius +1) - xa*xa;
	if ( radicant > 0 ) ya = sqrt( radicant );
//	qDebug() << "xa:" << xa << "ya:" << ya;
//	ya = (m_rlimit > xa*xa) ? sqrt((float)(m_rlimit) - (float)(xa*xa)) : 0;
//	qDebug() << " m_rlimit" << m_rlimit << " xa*xa" << xa*xa << " ya: " << ya;
	if ((currentPoint.y() - (imgry + 1)) < 0) ya = -ya; 

	return QPointF((float)imgrx + xa + 1, (float)imgry + ya + 1); 
}


void GridMap::resizeMap(const QPaintDevice * origimg){
	imgwidth = origimg -> width();

	imgheight = origimg -> height();
	imgrx = (imgwidth >> 1);
	imgry = (imgheight >> 1);
	imgradius = imgrx*imgrx + imgry*imgry;
}
