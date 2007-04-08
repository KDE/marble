#ifndef GRIDMAP_H
#define GRIDMAP_H

#include <QtGui/QPen>

#include "Quaternion.h"
#include "GeoPoint.h"
#include "ScreenPolygon.h"
#include "clippainter.h"


/**
@author Torsten Rahn
*/
class ClipPainter;

class GridMap : public ScreenPolygon::Vector {
public:
	GridMap();
	~GridMap();

	void createTropics( const int&, Quaternion& );
	void createGrid( const int&, Quaternion& );

	void paintGridMap(ClipPainter *, bool );

	void resizeMap(const QPaintDevice *);

	void setPen ( const QPen & p ){m_pen = p;}

	enum SphereDim {Longitude, Latitude};

private:
	void createCircles( int lngNum, int latNum );

	void createCircle( float, SphereDim, float cutcoeff = 0.0f );

	inline void initCrossHorizon();

	const QPointF horizonPoint();

private:
	matrix m_rotMatrix;

	ScreenPolygon m_polygon;

	int imgrx, imgry, imgradius;
	int imgwidth, imgheight;

	QPointF currentPoint, lastPoint; 

	QPen m_pen;

//	Dealing with the horizon
	bool lastvisible, currentlyvisible;
	
	int m_precision;
	int m_radius;
	int m_rlimit;
};

#endif // GRIDMAP_H
