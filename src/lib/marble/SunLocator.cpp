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

#include "MarbleGlobal.h"
#include "MarbleClock.h"
#include "Planet.h"
#include "MarbleMath.h"
 
#include "MarbleDebug.h"

#include "src/lib/astro/solarsystem.h"

#include <QDateTime>

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

class SunLocatorPrivate
{
public:
    SunLocatorPrivate( const MarbleClock *clock, const Planet *planet )
        : m_lon( 0.0 ),
          m_lat( 0.0 ),
          m_twilightZone( 0.0 ),
          m_clock( clock ),
          m_planet( planet )
    {
    }

    qreal m_lon;
    qreal m_lat;

    qreal m_twilightZone;

    const MarbleClock *const m_clock;
    const Planet *m_planet;
};


SunLocator::SunLocator( const MarbleClock *clock, const Planet *planet )
  : QObject(),
    d( new SunLocatorPrivate( clock, planet ))
{
    updateTwilightZone();
}

SunLocator::~SunLocator()
{
    delete d;
}

void SunLocator::updatePosition()
{
    QString planetId = d->m_planet->id();
    SolarSystem sys;

    QDateTime dateTime = d->m_clock->dateTime();
    sys.setCurrentMJD(
                dateTime.date().year(), dateTime.date().month(), dateTime.date().day(),
                dateTime.time().hour(), dateTime.time().minute(),
                (double)dateTime.time().second());
    QString const pname = planetId.at(0).toUpper() + planetId.right(planetId.size() - 1);
    QByteArray name = pname.toLatin1();
    sys.setCentralBody( name.data() );

    double ra = 0.0;
    double decl = 0.0;
    sys.getSun( ra, decl );
    double lon = 0.0;
    double lat = 0.0;
    sys.getPlanetographic (ra, decl, lon, lat);
    d->m_lon = lon * DEG2RAD;
    d->m_lat = lat * DEG2RAD;
}

void SunLocator::updateTwilightZone()
{
    const QString planetId = d->m_planet->id();

    if (planetId == QLatin1String("earth") || planetId == QLatin1String("venus")) {
        d->m_twilightZone = 0.1; // this equals 18 deg astronomical twilight.
    }
    else if (planetId == QLatin1String("mars")) {
        d->m_twilightZone = 0.05;
    }
    else {
        d->m_twilightZone = 0.0;
    }

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

    qreal brightness;
    if ( h <= 0.5 - d->m_twilightZone / 2.0 )
        brightness = 1.0;
    else if ( h >= 0.5 + d->m_twilightZone / 2.0 )
        brightness = 0.0;
    else
        brightness = ( 0.5 + d->m_twilightZone/2.0 - h ) / d->m_twilightZone;

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

    emit positionChanged( getLon(), getLat() );
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
    updateTwilightZone();
    updatePosition();

    // Initially there might be no planet set.
    // In that case we don't want an update.
    // Update the shading in all other cases.
    if ( !previousPlanet->id().isEmpty() ) {
        emit positionChanged( getLon(), getLat() );
    }
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

#include "moc_SunLocator.cpp"
