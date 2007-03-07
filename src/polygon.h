#ifndef POLYGON_H
#define POLYGON_H

#include <QtCore/QVector>
#include <QtGui/QPolygon>

class GeoPolygon : public QPolygonF {
public:
	GeoPolygon() : m_closePolyline(false) { }
	GeoPolygon(bool closePolyline) : m_closePolyline(closePolyline) { }
	~GeoPolygon() { }

	bool getClosed() const { return m_closePolyline; }
	void setClosed(bool closePolyline) { m_closePolyline = closePolyline; }

	// Type definitions
	typedef QVector<GeoPolygon> Vector;

protected:
	bool m_closePolyline : 1;	
};

#endif // POLYGON_H
