// (c) 2007-2008 David Roberts

#include "SunLocator.h"

#include <cmath>

SunLocator::SunLocator() {
	m_datetime = new ExtDateTime();
	updatePosition();
}

SunLocator::~SunLocator() {
}

void SunLocator::updatePosition() {
	// find orientation of sun
	
	m_datetime->update();
	const int J2000 = 2451545; // epoch J2000 = 1 January 2000, noon Terrestrial Time (11:58:55.816 UTC)
	long d = m_datetime->toJDN() - J2000; // find current Julian day number relative to epoch J2000
	
	// adapted from http://www.stargazing.net/kepler/sun.html
	double       L = 4.89497 + 0.0172028 * d;                  // mean longitude
	double       g = 6.24004 + 0.0172020 * d;                  // mean anomaly
	double  lambda = L + 0.0334 * sin(g) + 3.49e-4 * sin(2*g); // ecliptic longitude
	double epsilon = 0.40909 - 7e-9 * d;                       // obliquity of the ecliptic plane
	double   delta = asin(sin(epsilon)*sin(lambda));           // declination
	
	// convert position of sun to coordinates
	m_lat = -delta;
	m_lon = M_PI - m_datetime->dayFraction() * 2*M_PI;
}

double SunLocator::shading(double lat, double lon) {
	// haversine formula
	double a = sin((lat-m_lat)/2.0);
	double b = sin((lon-m_lon)/2.0);
	double h = (a*a)+cos(lat)*cos(m_lat)*(b*b);
	
	/*
	h = 0.0 // directly beneath sun
	h = 0.5 // sunrise/sunset line
	h = 1.0 // opposite side of earth to the sun
	theta = 2*asin(sqrt(h))
	*/
	
	return h;
}

QRgb SunLocator::shadePixel(QRgb pixcol, double shade) {
	const int factor = 200;
	const double penumbra = 0.02;
	
	if(shade <= 0.5 - penumbra/2.0)
		return pixcol;
	else if(shade >= 0.5 + penumbra/2.0)
		return QColor(pixcol).darker(factor + 100).rgb();
	else
		return QColor(pixcol).darker(int((shade - (0.5 - penumbra/2.0)) * factor/penumbra + 100)).rgb();
}
