// SPDX-FileCopyrightText: 2007-2009 David Roberts <dvdr18@gmail.com>
//
// SPDX-License-Identifier: LGPL-2.1-or-later

#include "SunLocator.h"

#include "MarbleClock.h"
#include "MarbleGlobal.h"
#include "MarbleMath.h"
#include "Planet.h"

#include "MarbleDebug.h"

#include <QDateTime>

#include <cmath>

namespace Marble
{

using std::sin;

class SunLocatorPrivate
{
public:
    SunLocatorPrivate(const MarbleClock *clock, const Planet *planet)
        : m_lon(0.0)
        , m_lat(0.0)
        , m_twilightZone(planet->twilightZone())
        , m_clock(clock)
        , m_planet(planet)
    {
        planet->sunPosition(m_lon, m_lat, clock->dateTime());
    }

    qreal m_lon;
    qreal m_lat;

    qreal m_twilightZone;

    const MarbleClock *const m_clock;
    const Planet *m_planet;
};

SunLocator::SunLocator(const MarbleClock *clock, const Planet *planet)
    : QObject()
    , d(new SunLocatorPrivate(clock, planet))
{
}

SunLocator::~SunLocator()
{
    delete d;
}

qreal SunLocator::shading(qreal lon, qreal a, qreal c) const
{
    // haversine formula
    qreal b = sin((lon - d->m_lon) / 2.0);
    //    qreal g = sin((lat-d->m_lat)/2.0);
    //    qreal h = (g*g)+cos(lat)*cos(d->m_lat)*(b*b);
    qreal h = (a * a) + c * (b * b);

    /*
      h = 0.0 // directly beneath sun
      h = 0.5 // sunrise/sunset line
      h = 1.0 // opposite side of earth to the sun
      theta = 2*asin(sqrt(h))
    */

    qreal brightness;
    if (h <= 0.5 - d->m_twilightZone / 2.0)
        brightness = 1.0;
    else if (h >= 0.5 + d->m_twilightZone / 2.0)
        brightness = 0.0;
    else
        brightness = (0.5 + d->m_twilightZone / 2.0 - h) / d->m_twilightZone;

    return brightness;
}

void SunLocator::shadePixel(QRgb &pixcol, qreal brightness)
{
    // daylight - no change
    if (brightness > 0.99999)
        return;

    if (brightness < 0.00001) {
        // night
        //      Doing  "pixcol = qRgb(r/2, g/2, b/2);" by shifting some electrons around ;)
        // by shifting some electrons around ;)
        pixcol = qRgb(qRed(pixcol) * 0.35, qGreen(pixcol) * 0.35, qBlue(pixcol) * 0.35);
        // pixcol = (pixcol & 0xff000000) | ((pixcol >> 1) & 0x7f7f7f);
    } else {
        // gradual shadowing
        int r = qRed(pixcol);
        int g = qGreen(pixcol);
        int b = qBlue(pixcol);
        qreal d = 0.65 * brightness + 0.35;
        pixcol = qRgb((int)(d * r), (int)(d * g), (int)(d * b));
    }
}

void SunLocator::shadePixelComposite(QRgb &pixcol, const QRgb &dpixcol, qreal brightness)
{
    // daylight - no change
    if (brightness > 0.99999)
        return;

    if (brightness < 0.00001) {
        // night
        pixcol = dpixcol;
    } else {
        // gradual shadowing
        qreal &d = brightness;

        int r = qRed(pixcol);
        int g = qGreen(pixcol);
        int b = qBlue(pixcol);

        int dr = qRed(dpixcol);
        int dg = qGreen(dpixcol);
        int db = qBlue(dpixcol);

        pixcol = qRgb((int)(d * r + (1 - d) * dr), (int)(d * g + (1 - d) * dg), (int)(d * b + (1 - d) * db));
    }
}

void SunLocator::update()
{
    d->m_planet->sunPosition(d->m_lon, d->m_lat, d->m_clock->dateTime());

    Q_EMIT positionChanged(getLon(), getLat());
}

void SunLocator::setPlanet(const Planet *planet)
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
    d->m_twilightZone = planet->twilightZone();
    planet->sunPosition(d->m_lon, d->m_lat, d->m_clock->dateTime());

    // Initially there might be no planet set.
    // In that case we don't want an update.
    // Update the shading in all other cases.
    if (!previousPlanet->id().isEmpty()) {
        Q_EMIT positionChanged(getLon(), getLat());
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
