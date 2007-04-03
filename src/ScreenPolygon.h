#ifndef POLYGON_H
#define POLYGON_H

#include <QtCore/QVector>
#include <QtGui/QPolygon>


class ScreenPolygon : public QPolygonF {
public:
	ScreenPolygon() : m_closePolyline(false) { }
	ScreenPolygon(bool closePolyline) : m_closePolyline(closePolyline) { }
	~ScreenPolygon() { }

	bool getClosed() const { return m_closePolyline; }
	void setClosed(bool closePolyline) { m_closePolyline = closePolyline; }

	// Type definitions
	typedef QVector<ScreenPolygon> Vector;

protected:
	bool m_closePolyline : 1;	
};

#endif // POLYGON_H
