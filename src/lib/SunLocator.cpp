// Copyright 2007-2009 David Roberts <dvdr18@gmail.com>
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

#include "global.h"
#include "MarbleClock.h"
#include "Planet.h"
#include "MarbleMath.h"
 
#include "MarbleDebug.h"

#include <cmath>
// M_PI is sometimes defined in <cmath>
#ifndef M_PI 
#define M_PI 3.14159265358979323846264338327950288419717
#endif

namespace Marble
{

using std::sin;
using std::cos;
using std::asin;
using std::abs;


// epoch J2000 = 1 January 2000, noon Terrestrial Time (11:58:55.816 UTC)
const int J2000 = 2451545;

// taking the full moon of 15 January 1900 19:07 UTC as the epoch for the moon
// value from http://home.hiwaay.net/~krcool/Astro/moon/fullmoon.htm
const qreal MOON_EPOCH = 2415035.297;
const qreal MOON_SYNODIC_PERIOD = 29.530588;

// emit updateSun() every update_interval ms
const int update_interval = 60000;

class SunLocatorPrivate
{
public:
    SunLocatorPrivate( const MarbleClock *clock, const Planet *planet )
        : m_lon( 0.0 ),
          m_lat( 0.0 ),
          m_clock( clock ),
          m_centered( false ),
          m_planet( planet )
    {
    }

    qreal m_lon;
    qreal m_lat;

    const MarbleClock *const m_clock;
    bool m_centered;
    const Planet *m_planet;
};


SunLocator::SunLocator( const MarbleClock *clock, const Planet *planet )
  : QObject(),
    d( new SunLocatorPrivate( clock, planet ))
{
}

SunLocator::~SunLocator()
{
    delete d;
}

void SunLocator::updatePosition()
{
    if( d->m_planet->id() == "moon" ) {
        // days since the first full moon of the 20th century
        qreal days = (qreal)d->m_clock->dateTime().date().toJulianDay() + d->m_clock->dayFraction() - MOON_EPOCH;

        // number of orbits the moon has made (relative to the sun as observed from earth)
        days /= MOON_SYNODIC_PERIOD;

        // take fractional part
        days = days - (int)days;

        // for dates before MOON_EPOCH
        if (days < 0.0)
            days += 1.0;

        mDebug() << "MOON:" << (int)(days*100) << "% of orbit completed and"
                 << (int)(abs((days-0.5)*2) * 100) << "% illuminated";

        d->m_lon = (1-days) * 2*M_PI;

        // not necessarily accurate but close enough
        // (only differs by about +-6 degrees of this value)
        d->m_lat = 0.0;
        return;
    }

    // find current Julian day number relative to epoch J2000
    long day = d->m_clock->dateTime().date().toJulianDay() - J2000;

    // from http://www.astro.uu.nl/~strous/AA/en/reken/zonpositie.html
    // mean anomaly
    qreal M = d->m_planet->M_0() + d->m_planet->M_1()*day;

    // equation of center
    qreal C = d->m_planet->C_1()*sin(M) + d->m_planet->C_2()*sin(2*M)
            + d->m_planet->C_3()*sin(3*M) + d->m_planet->C_4()*sin(4*M)
            + d->m_planet->C_5()*sin(5*M) + d->m_planet->C_6()*sin(6*M);

    // true anomaly
    qreal nu = M + C;

    // ecliptic longitude of sun as seen from planet
    qreal lambda_sun = nu + d->m_planet->Pi() + M_PI;

    // declination of sun as seen from planet
    qreal delta_sun = asin(sin(d->m_planet->epsilon())*sin(lambda_sun));

    // right ascension of sun as seen from planet
    qreal alpha_sun = atan2(cos(d->m_planet->epsilon())*sin(lambda_sun), cos(lambda_sun));

    // solar noon occurs when sidereal time is equal to alpha_sun
    qreal theta = alpha_sun;

    // convert sidereal time to geographic longitude
    d->m_lon = M_PI - (d->m_planet->theta_0() + d->m_planet->theta_1()
                       * (day + d->m_clock->dayFraction()) - theta);

    while(d->m_lon < 0)
        d->m_lon += 2*M_PI;

    d->m_lat = delta_sun;
}


qreal SunLocator::shading(qreal lon, qreal a, qreal c) const
{
    // haversine formula
    qreal b = sin((lon-d->m_lon)/2.0);
//    qreal g = sin((lat-d->m_lat)/2.0);
//    qreal h = (g*g)+cos(lat)*cos(d->m_lat)*(b*b); 
    qreal h = (a*a) + c * (b*b); 

    /*
      h = 0.0 // directly beneath sun
      h = 0.5 // sunrise/sunset line
      h = 1.0 // opposite side of earth to the sun
      theta = 2*asin(sqrt(h))
    */

    qreal twilightZone = 0.0;

    if ( d->m_planet->id() == "earth" || d->m_planet->id() == "venus" ) {
        twilightZone = 0.1; // this equals 18 deg astronomical twilight.
    }

    qreal brightness;
    if ( h <= 0.5 - twilightZone / 2.0 )
        brightness = 1.0;
    else if ( h >= 0.5 + twilightZone / 2.0 )
        brightness = 0.0;
    else
        brightness = ( 0.5 + twilightZone/2.0 - h ) / twilightZone;

    return brightness;
}

void SunLocator::shadePixel(QRgb& pixcol, qreal brightness) const
{
    // daylight - no change
    if ( brightness > 0.99999 )
        return;

    if ( brightness < 0.00001 ) {
        // night
        //      Doing  "pixcol = qRgb(r/2, g/2, b/2);" by shifting some electrons around ;)
        // by shifting some electrons around ;)
        pixcol = qRgb(qRed(pixcol) * 0.35, qGreen(pixcol) * 0.35, qBlue(pixcol)  * 0.35);
        // pixcol = (pixcol & 0xff000000) | ((pixcol >> 1) & 0x7f7f7f);
    } else {
        // gradual shadowing
        int r = qRed( pixcol );
        int g = qGreen( pixcol );
        int b = qBlue( pixcol );
        qreal  d = 0.65 * brightness + 0.35;
        pixcol = qRgb((int)(d * r), (int)(d * g), (int)(d * b));
    }
}

void SunLocator::shadePixelComposite(QRgb& pixcol, const QRgb& dpixcol,
                                     qreal brightness) const
{
    // daylight - no change
    if ( brightness > 0.99999 )
        return;

    if ( brightness < 0.00001 ) {
        // night
        pixcol = dpixcol;
    } else {
        // gradual shadowing
        qreal& d = brightness;

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

    emit updateSun();

    if ( d->m_centered ) {
        emit centerSun( getLon(), getLat() );
        return;
    }

    emit updateStars();
}

void SunLocator::setCentered(bool centered)
{
    if ( centered == d->m_centered ) {
        return;
    }

    mDebug() << "SunLocator::setCentered";
    mDebug() << "sunLocator =" << this;
    d->m_centered = centered;
    if ( d->m_centered ) {
        emit centerSun( getLon(), getLat() );
    }
}

void SunLocator::setPlanet( const Planet *planet )
{
    /*
    // This won't work as expected if the same pointer 
    // points to different planets
    if ( planet == d->m_planet ) {
        return;
    }
    */

    const Planet *previousPlanet = d->m_planet;

    mDebug() << "SunLocator::setPlanet(Planet*)";
    d->m_planet = planet;
    updatePosition();

    // Initially there might be no planet set.
    // In that case we don't want an update.
    // Update the shading in all other cases.
    if ( !previousPlanet->id().isEmpty() ) {
        emit updateSun();
    }
}

bool SunLocator::getCentered() const
{
    return d->m_centered;
}

qreal SunLocator::getLon() const
{
    return d->m_lon * RAD2DEG;
}

qreal SunLocator::getLat() const
{
    return d->m_lat * RAD2DEG;
}

}

#include "SunLocator.moc"
