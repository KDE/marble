#ifndef GEOPOINT_H
#define GEOPOINT_H

#include <QtCore/QVector>

#include "Quaternion.h"


class GeoPoint {
public:
	GeoPoint(){}
	GeoPoint(int, int);
	GeoPoint(float, float);
	GeoPoint(int, int, int);
	~GeoPoint(){}
	
	int detail()  const { return m_detail; }

	void geoCoordinates( float& lon, float& lat ) const {
	    m_q.getSpherical( lon, lat ); 
	}

	const Quaternion &quaternion() const { return m_q; }
	QString toString();

	// Type definitions
	typedef QVector<GeoPoint> Vector;

private:	
	Quaternion  m_q;
	int         m_detail;

};

#endif // GEOPOINT_H
