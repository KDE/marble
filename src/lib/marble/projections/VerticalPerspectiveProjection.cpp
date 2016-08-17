//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2014      Torsten Rahn <rahn@kde.org>
//

// Local
#include "VerticalPerspectiveProjection.h"
#include "AbstractProjection_p.h"

#include "MarbleDebug.h"

// Marble
#include "ViewportParams.h"
#include "GeoDataPoint.h"
#include "GeoDataLineString.h"
#include "GeoDataCoordinates.h"
#include "MarbleGlobal.h"
#include "AzimuthalProjection_p.h"

#include <QIcon>
#include <qmath.h>

#define SAFE_DISTANCE

namespace Marble
{

class VerticalPerspectiveProjectionPrivate : public AzimuthalProjectionPrivate
{
  public:
    explicit VerticalPerspectiveProjectionPrivate( VerticalPerspectiveProjection * parent );

    void calculateConstants(qreal radius) const;

    mutable qreal m_P; ///< Distance of the point of perspective in earth diameters
    mutable qreal m_previousRadius;
    mutable qreal m_altitudeToPixel;
    mutable qreal m_perspectiveRadius;
    mutable qreal m_pPfactor;

    Q_DECLARE_PUBLIC( VerticalPerspectiveProjection )
};

VerticalPerspectiveProjection::VerticalPerspectiveProjection()
    : AzimuthalProjection( new VerticalPerspectiveProjectionPrivate( this ) )
{
    setMinLat( minValidLat() );
    setMaxLat( maxValidLat() );
}

VerticalPerspectiveProjection::VerticalPerspectiveProjection( VerticalPerspectiveProjectionPrivate *dd )
        : AzimuthalProjection( dd )
{
    setMinLat( minValidLat() );
    setMaxLat( maxValidLat() );
}

VerticalPerspectiveProjection::~VerticalPerspectiveProjection()
{
}


VerticalPerspectiveProjectionPrivate::VerticalPerspectiveProjectionPrivate( VerticalPerspectiveProjection * parent )
        : AzimuthalProjectionPrivate( parent ),
          m_P(1),
          m_previousRadius(1),
          m_altitudeToPixel(1),
          m_perspectiveRadius(1),
          m_pPfactor(1)
{
}


QString VerticalPerspectiveProjection::name() const
{
    return QObject::tr( "Vertical Perspective Projection" );
}

QString VerticalPerspectiveProjection::description() const
{
    return QObject::tr( "<p><b>Vertical Perspective Projection</b> (\"orthogonal\")</p><p> Shows the earth as it appears from a relatively short distance above the surface. Applications: Used for Virtual Globes.</p>" );
}

QIcon VerticalPerspectiveProjection::icon() const
{
    return QIcon(QStringLiteral(":/icons/map-globe.png"));
}

void VerticalPerspectiveProjectionPrivate::calculateConstants(qreal radius) const
{
    if (radius == m_previousRadius)  return;
    m_previousRadius = radius;
    m_P = 1.5 + 3 * 1000 * 0.4 / radius / qTan(0.5 * 110 * DEG2RAD);
    m_altitudeToPixel = radius / (EARTH_RADIUS * qSqrt((m_P-1)/(m_P+1)));
    m_perspectiveRadius = radius / qSqrt((m_P-1)/(m_P+1));
    m_pPfactor = (m_P+1)/(m_perspectiveRadius*m_perspectiveRadius*(m_P-1));
}

qreal VerticalPerspectiveProjection::clippingRadius() const
{
    return 1;
}

bool VerticalPerspectiveProjection::screenCoordinates( const GeoDataCoordinates &coordinates,
                                             const ViewportParams *viewport,
                                             qreal &x, qreal &y, bool &globeHidesPoint ) const
{
    Q_D(const VerticalPerspectiveProjection);
    d->calculateConstants(viewport->radius());
    const qreal P =  d->m_P;
    const qreal deltaLambda = coordinates.longitude() - viewport->centerLongitude();
    const qreal phi = coordinates.latitude();
    const qreal phi1 = viewport->centerLatitude();

    qreal cosC = qSin( phi1 ) * qSin( phi ) + qCos( phi1 ) * qCos( phi ) * qCos( deltaLambda );

    // Don't display placemarks that are below 10km altitude and
    // are on the Earth's backside (where cosC < 1/P)
    if (cosC < 1/P && coordinates.altitude() < 10000) {
        globeHidesPoint = true;
        return false;
    }

    // Let (x, y) be the position on the screen of the placemark ..
    // First determine the position in unit coordinates:
    qreal k = (P - 1) / (P - cosC); // scale factor
    x = ( qCos( phi ) * qSin( deltaLambda ) ) * k;
    y = ( qCos( phi1 ) * qSin( phi ) - qSin( phi1 ) * qCos( phi ) * qCos( deltaLambda ) ) * k;

    // Transform to screen coordinates
    qreal pixelAltitude = (coordinates.altitude() + EARTH_RADIUS) * d->m_altitudeToPixel;
    x *= pixelAltitude;
    y *= pixelAltitude;


    // Don't display satellites that are on the Earth's backside:
    if (cosC < 1/P && x*x+y*y < viewport->radius() * viewport->radius()) {
        globeHidesPoint = true;
        return false;
    }
    // The remaining placemarks are definetely not on the Earth's backside
    globeHidesPoint = false;

    x += viewport->width() / 2;
    y = viewport->height() / 2 - y;

    // Skip placemarks that are outside the screen area
    if ( x < 0 || x >= viewport->width() || y < 0 || y >= viewport->height() ) {
        return false;
    }

    return true;
}

bool VerticalPerspectiveProjection::screenCoordinates( const GeoDataCoordinates &coordinates,
                                             const ViewportParams *viewport,
                                             qreal *x, qreal &y,
                                             int &pointRepeatNum,
                                             const QSizeF& size,
                                             bool &globeHidesPoint ) const
{
    pointRepeatNum = 0;
    globeHidesPoint = false;

    bool visible = screenCoordinates( coordinates, viewport, *x, y, globeHidesPoint );

    // Skip placemarks that are outside the screen area
    if ( *x + size.width() / 2.0 < 0.0 || *x >= viewport->width() + size.width() / 2.0
         || y + size.height() / 2.0 < 0.0 || y >= viewport->height() + size.height() / 2.0 )
    {
        return false;
    }

    // This projection doesn't have any repetitions,
    // so the number of screen points referring to the geopoint is one.
    pointRepeatNum = 1;
    return visible;
}


bool VerticalPerspectiveProjection::geoCoordinates( const int x, const int y,
                                          const ViewportParams *viewport,
                                          qreal& lon, qreal& lat,
                                          GeoDataCoordinates::Unit unit ) const
{
    Q_D(const VerticalPerspectiveProjection);
    d->calculateConstants(viewport->radius());
    const qreal P = d->m_P;
    const qreal rx = ( - viewport->width()  / 2 + x );
    const qreal ry = (   viewport->height() / 2 - y );
    const qreal p2 = rx*rx + ry*ry;

    if (p2 == 0) {
        lon = viewport->centerLongitude();
        lat = viewport->centerLatitude();
        return true;
    }

    const qreal pP = p2*d->m_pPfactor;

    if ( pP > 1) return false;

    const qreal p = qSqrt(p2);
    const qreal fract = d->m_perspectiveRadius*(P-1)/p;
    const qreal c = qAsin((P-qSqrt(1-pP))/(fract+1/fract));
    const qreal sinc = qSin(c);

    const qreal centerLon = viewport->centerLongitude();
    const qreal centerLat = viewport->centerLatitude();
    lon = centerLon + qAtan2(rx*sinc, (p*qCos(centerLat)*qCos(c) - ry*qSin(centerLat)*sinc));

    while ( lon < -M_PI ) lon += 2 * M_PI;
    while ( lon >  M_PI ) lon -= 2 * M_PI;

    lat = qAsin(qCos(c)*qSin(centerLat) + (ry*sinc*qCos(centerLat))/p);

    if ( unit == GeoDataCoordinates::Degree ) {
        lon *= RAD2DEG;
        lat *= RAD2DEG;
    }

    return true;
}

}
