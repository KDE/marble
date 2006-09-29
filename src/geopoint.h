#ifndef GEOPOINT_H
#define GEOPOINT_H

#include <QVector>
#include "quaternion.h"

class GeoPoint {
private:	
	int m_Detail, m_Lat, m_Lng;
	Quaternion m_q;
	Quaternion m_mulq;
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
	void setMul( float radius ){ m_mulq = m_q; m_mulq.scalar( radius ); }

	const Quaternion &getQuatPoint() const { return m_q; }

	// Type definitions
	typedef QVector<GeoPoint> Vector;
};

#endif // GEOPOINT_H
