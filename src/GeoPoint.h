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
	
	int detail()  const { return m_detail; }
	int lat()     const { return m_lat; }
	int lon()     const { return m_lon; }
#if 0
	void geoCoordinates( float& lon, float& lat ) const {
	    // FIXME: This copy is only necessary because
	    //        q.getSpherical() is not a const method.
	    Quaternion  q = m_q;
	    q.getSpherical( lon, lat ); 
	}
#else
	void geoCoordinates( float& lon, float& lat ) {
	    m_q.getSpherical( lon, lat ); 
	}
#endif
	const Quaternion &quaternion() const { return m_q; }
	QString toString();

	// Type definitions
	typedef QVector<GeoPoint> Vector;

private:	
	Quaternion  m_q;
	int         m_detail;

	int         m_lat; // the use of these is deprecated
	int         m_lon;
};

#endif // GEOPOINT_H
