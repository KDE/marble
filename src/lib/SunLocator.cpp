// Copyright 2007-2008 David Roberts
// 
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either 
// version 2.1 of the License, or (at your option) any later version.
// 
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public 
// License along with this library.  If not, see <http://www.gnu.org/licenses/>.


#include "SunLocator.h"
#include "ExtDateTime.h"

#include <QtCore/QDebug>

using std::sin;
using std::cos;
using std::asin;

const int J2000 = 2451545; // epoch J2000 = 1 January 2000, noon Terrestrial Time (11:58:55.816 UTC)
const double twilightZone = 0.1; // this equals 18 deg astronomical twilight.
const int update_interval = 60000; // emit updateSun() every update_interval ms

SunLocator::SunLocator(ExtDateTime *dateTime)
  : QObject(),
    m_datetime( dateTime ),
    m_show( false ),
    m_citylights( false ),
    m_centered( false )
{
}

SunLocator::~SunLocator() {
//     delete m_datetime;
}

void SunLocator::updatePosition()
{
    // Find the orientation of the sun.
    // Find current Julian day number relative to epoch J2000.
    long d = m_datetime->toJDN() - J2000;
	
    // Adapted from http://www.stargazing.net/kepler/sun.html
    double       L = 4.89497 + 0.0172028 * d;                  // mean longitude
    double       g = 6.24004 + 0.0172020 * d;                  // mean anomaly
    double  lambda = L + 0.0334 * sin(g) + 3.49e-4 * sin(2*g); // ecliptic longitude
    double epsilon = 0.40909 - 7e-9 * d;                       // obliquity of the ecliptic plane
    double   delta = asin(sin(epsilon)*sin(lambda));           // declination
	
    // Convert position of sun to coordinates.
    m_lon = M_PI - m_datetime->dayFraction() * 2*M_PI;
    m_lat = -delta;
}


double SunLocator::shading(double lon, double lat)
{
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
	
    double brightness;
    if ( h <= 0.5 - twilightZone / 2.0 )
        brightness = 1.0;
    else if ( h >= 0.5 + twilightZone / 2.0 )
        brightness = 0.0;
    else
        brightness = ( 0.5 + twilightZone/2.0 - h ) / twilightZone;
	
    return brightness;
}

void SunLocator::shadePixel(QRgb& pixcol, double brightness)
{
    // daylight - no change
    if ( brightness > 0.99999 )
        return;
		
    if ( brightness < 0.00001 ) {
        // night
//      Doing  "pixcol = qRgb(r/2, g/2, b/2);" by shifting some electrons around ;)
        pixcol = (pixcol & 0xff000000) | ((pixcol >> 1) & 0x7f7f7f);
    } else {
        // gradual shadowing
        int r = qRed( pixcol );
        int g = qGreen( pixcol );
        int b = qBlue( pixcol );
        double  d = 0.5 * brightness + 0.5;
        pixcol = qRgb((int)(d * r), (int)(d * g), (int)(d * b));
    }
}

void SunLocator::shadePixelComposite(QRgb& pixcol, QRgb& dpixcol,
                                     double brightness)
{
    // daylight - no change
    if ( brightness > 0.99999 )
        return;
	
    if ( brightness < 0.00001 ) {
        // night
        pixcol = dpixcol;
    } else {
        // gradual shadowing
        double& d = brightness;
		
        int r = qRed( pixcol );
        int g = qGreen( pixcol );
        int b = qBlue( pixcol );
		
        int dr = qRed( dpixcol );
        int dg = qGreen( dpixcol );
        int db = qBlue( dpixcol );
		
        pixcol = qRgb( (int)( d * r + (1 - d) * dr ),
                       (int)( d * g + (1 - d) * dg ),
                       (int)( d * b + (1 - d) * db ) );
    }
}

void SunLocator::update()
{
    updatePosition();
    if ( m_show )
        emit updateSun();
    if ( m_centered )
        emit centerSun();
}

void SunLocator::setShow(bool show)
{
    m_show = show;
    updatePosition();

    emit updateSun();
}

void SunLocator::setCentered(bool centered)
{
    qDebug() << "SunLocator::setCentered";
    qDebug() << "sunLocator =" << this;
    m_centered = centered;
    if ( m_centered ) {
        updatePosition();
        emit centerSun();
    } else
        emit reenableWidgetInput();
}

#include "SunLocator.moc"
