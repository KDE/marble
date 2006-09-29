#ifndef VECTORMAP_H
#define VECTORMAP_H

#include <QPen>
#include <QBrush>
#include <QImage>
#include <QPixmap>

#include "geopoint.h"
#include "polygon.h"
#include "quaternion.h"

/**
@author Torsten Rahn
*/
class PntMap;
class ClipPainter;

class VectorMap : public GeoPolygon::Vector {
public:
	VectorMap();
	~VectorMap();
	void createFromPntMap(const PntMap*, const int&, Quaternion&);
	void paintMap(ClipPainter *, bool );
	void drawMap(QPaintDevice *, bool );
	void resizeMap(const QPaintDevice *);

	void setPen ( const QPen & p ){m_pen = p;}
	void setBrush ( const QBrush & b ){m_brush = b;}
	void setzBoundingBoxLimit ( const float zBoundingBoxLimit ) { m_zBoundingBoxLimit = zBoundingBoxLimit; }
	void setzPointLimit ( const float zPointLimit ) { m_zPointLimit = zPointLimit; }
private:
	GeoPoint::Vector boundary;
	void createPolyLine(GeoPoint::Vector::ConstIterator, GeoPoint::Vector::ConstIterator, const int);
	float m_zlimit, m_plimit;
	float m_zBoundingBoxLimit;	
	float m_zPointLimit;	
//	Quaternion m_invRotAxis;
	matrix m_rotMatrix;

	GeoPolygon m_polygon;
	void initCrossHorizon();
	void manageCrossHorizon();
	const QPoint horizonPoint();
	void createArc();

	void initCrossFrame();
	void manageCrossFrame();
	const QPoint framePoint();
	void createBorder();

	int imgrx, imgry, imgradius;
	int imgwidth, imgheight;

	QPoint currentPoint, lastPoint; 

	QPen m_pen;
	QBrush m_brush;
// Dealing with the horizon
	bool firsthorizon;
	bool lastvisible, currentlyvisible, horizonpair;
	QPoint firstHorizonPoint, horizona, horizonb;
	
	int m_radius;
	int m_rlimit;

	int getDetailLevel() const;
};

#endif // VECTORMAP_H
