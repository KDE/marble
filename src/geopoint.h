#ifndef GEOPOINT_H
#define GEOPOINT_H

#include <QtCore/QVector>
#include "quaternion.h"

class GeoPoint {
private:	
	Quaternion m_q;
	int m_Detail;
	int m_Lat, m_Lng; // the use of these is deprecated

public:
	GeoPoint(){}
	GeoPoint(int, int);
	GeoPoint(float, float);
	GeoPoint(int, int, int);
	~GeoPoint(){}
	
	int getDetail() const { return m_Detail; }
	int getLat() const { return m_Lat; }
	int getLng() const { return m_Lng; }
	void geoCoordinates( float& lng, float& lat ){ m_q.getSpherical( lng, lat ); }

	const Quaternion &getQuatPoint() const { return m_q; }
	QString toString();

	// Type definitions
	typedef QVector<GeoPoint> Vector;
};

#endif // GEOPOINT_H
