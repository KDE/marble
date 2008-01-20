// (c) 2007--2008 David Roberts

#ifndef SUNLOCATOR_H
#define SUNLOCATOR_H

#include "ExtDateTime.h"

#include <QtCore/QObject>
#include <QtGui/QColor>
#include <QTimer>

#include <cmath>

class SunLocator : public QObject {
	Q_OBJECT
	
	public:
	explicit SunLocator();
	virtual ~SunLocator();
	void updatePosition();
	double shading(double lat, double lon);
	void shadePixel(QRgb& pixcol, double shade);
	void shadePixelComposite(QRgb& pixcol, QRgb& dpixcol, double shade);
	
	void setShow(bool show) {m_show = show;}
	
	bool getShow() {return m_show;}
	double getLat() {return -m_lat * 180.0 / M_PI;}
	double getLon() {return m_lon * 180.0 / M_PI;}
	
	ExtDateTime* datetime() {return m_datetime;}
	
	private Q_SLOTS:
	void timerTimeout();
	
	Q_SIGNALS:
	void updateSun();
	
	protected:
	double m_lat;
	double m_lon;
	ExtDateTime* m_datetime;
	bool m_show;
	QTimer* m_timer;
};

#endif
