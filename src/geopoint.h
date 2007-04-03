#ifndef GEOPOINT_H
#define GEOPOINT_H

#include <QtCore/QVector>
#include "quaternion.h"

class GeoPoint {
public:
	GeoPoint(){}
	GeoPoint(int, int);
	GeoPoint(float, float);
	GeoPoint(int, int, int);
	~GeoPoint(){}
	
	int getDetail()  const { return m_Detail; }
	int lat()        const { return m_lat; }
	int lon()        const { return m_lon; }
	void geoCoordinates( float& lon, float& lat ) { m_q.getSpherical( lon, lat ); }

	const Quaternion &getQuatPoint() const { return m_q; }
	QString toString();

	// Type definitions
	typedef QVector<GeoPoint> Vector;

private:	
	Quaternion  m_q;
	int         m_Detail;

	int         m_lat; // the use of these is deprecated
	int         m_lon;
};

#endif // GEOPOINT_H
