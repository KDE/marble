// (c) 2007--2008 David Roberts

#ifndef SUNLOCATOR_H
#define SUNLOCATOR_H

#include "ExtDateTime.h"

#include <QtGui/QColor>

class SunLocator {
	public:
		explicit SunLocator();
		virtual ~SunLocator();
		void updatePosition();
		double shading(double lat, double lon);
		void shadePixel(QRgb& pixcol, double shade);
		void shadePixelComposite(QRgb& pixcol, QRgb& dpixcol, double shade);
	
	protected:
		double m_lat;
		double m_lon;
		ExtDateTime* m_datetime;
};

#endif
