#ifndef GRIDMAP_H
#define GRIDMAP_H

#include <QPen>
#include <QBrush>
#include <QImage>
#include <QPixmap>

#include "clippainter.h"
#include "geopoint.h"
#include "polygon.h"
#include "quaternion.h"

/**
@author Torsten Rahn
*/
class ClipPainter;

class GridMap : public GeoPolygon::Vector {
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

	matrix m_rotMatrix;

	GeoPolygon m_polygon;

	inline void initCrossHorizon();

	const QPointF horizonPoint();

	int imgrx, imgry, imgradius;
	int imgwidth, imgheight;

	QPointF currentPoint, lastPoint; 

	QPen m_pen;
	QBrush m_brush;
// Dealing with the horizon
	bool firsthorizon;
	bool lastvisible, currentlyvisible, horizonpair;
	QPoint horizona, horizonb;
	
	int m_precision;
	int m_radius;
	int m_rlimit;

};

#endif // GRIDMAP_H
