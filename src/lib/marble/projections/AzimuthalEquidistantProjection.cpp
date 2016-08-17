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
#include "AzimuthalEquidistantProjection.h"
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

class AzimuthalEquidistantProjectionPrivate : public AzimuthalProjectionPrivate
{
  public:
    explicit AzimuthalEquidistantProjectionPrivate( AzimuthalEquidistantProjection * parent );

    Q_DECLARE_PUBLIC( AzimuthalEquidistantProjection )
};

AzimuthalEquidistantProjection::AzimuthalEquidistantProjection()
    : AzimuthalProjection( new AzimuthalEquidistantProjectionPrivate( this ) )
{
    setMinLat( minValidLat() );
    setMaxLat( maxValidLat() );
}

AzimuthalEquidistantProjection::AzimuthalEquidistantProjection( AzimuthalEquidistantProjectionPrivate *dd )
        : AzimuthalProjection( dd )
{
    setMinLat( minValidLat() );
    setMaxLat( maxValidLat() );
}

AzimuthalEquidistantProjection::~AzimuthalEquidistantProjection()
{
}

QString AzimuthalEquidistantProjection::name() const
{
    return QObject::tr( "Azimuthal Equidistant" );
}

QString AzimuthalEquidistantProjection::description() const
{
    return QObject::tr( "<p><b>Azimuthal Equidistant Projection</b> (\"fish eye\")</p><p>Applications: Display of seismic and radio data and for use in digital planetariums.</p>" );
}

QIcon AzimuthalEquidistantProjection::icon() const
{
    return QIcon(QStringLiteral(":/icons/map-globe.png"));
}

AzimuthalEquidistantProjectionPrivate::AzimuthalEquidistantProjectionPrivate( AzimuthalEquidistantProjection * parent )
        : AzimuthalProjectionPrivate( parent )
{
}

qreal AzimuthalEquidistantProjection::clippingRadius() const
{
    return 1;
}

bool AzimuthalEquidistantProjection::screenCoordinates( const GeoDataCoordinates &coordinates,
                                             const ViewportParams *viewport,
                                             qreal &x, qreal &y, bool &globeHidesPoint ) const
{
    const qreal lambda = coordinates.longitude();
    const qreal phi = coordinates.latitude();
    const qreal lambdaPrime = viewport->centerLongitude();
    const qreal phi1 = viewport->centerLatitude();

    qreal cosC = qSin( phi1 ) * qSin( phi ) + qCos( phi1 ) * qCos( phi ) * qCos( lambda - lambdaPrime );
    // Prevent division by zero
    if (cosC <= 0) {
        globeHidesPoint = true;
        return false;
    }

    qreal c = qAcos(cosC);

    qreal k = cosC == 1 ? 1 : c / qSin( c );

    // Let (x, y) be the position on the screen of the placemark..
    x = ( qCos( phi ) * qSin( lambda - lambdaPrime ) ) * k;
    y = ( qCos( phi1 ) * qSin( phi ) - qSin( phi1 ) * qCos( phi ) * qCos( lambda - lambdaPrime ) ) * k;

    x *= 2 * viewport->radius() / M_PI;
    y *= 2 * viewport->radius() / M_PI;

    const qint64  radius  = clippingRadius() * viewport->radius();

    if (x*x + y*y > radius * radius) {
        globeHidesPoint = true;
        return false;
    }

    globeHidesPoint = false;

    x += viewport->width() / 2;
    y = viewport->height() / 2 - y;

    // Skip placemarks that are outside the screen area
    if ( x < 0 || x >= viewport->width() || y < 0 || y >= viewport->height() ) {
        return false;
    }

    return true;
}

bool AzimuthalEquidistantProjection::screenCoordinates( const GeoDataCoordinates &coordinates,
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


bool AzimuthalEquidistantProjection::geoCoordinates( const int x, const int y,
                                          const ViewportParams *viewport,
                                          qreal& lon, qreal& lat,
                                          GeoDataCoordinates::Unit unit ) const
{
    const qint64  radius  = viewport->radius();
    // Calculate how many degrees are being represented per pixel.
    const qreal rad2Pixel = ( 2 * radius ) / M_PI;
    const qreal centerLon = viewport->centerLongitude();
    const qreal centerLat = viewport->centerLatitude();
    const qreal rx = ( - viewport->width()  / 2 + x ) / rad2Pixel;
    const qreal ry = (   viewport->height() / 2 - y ) / rad2Pixel;
    const qreal c = qMax( qSqrt( rx*rx + ry*ry ), qreal(0.0001) ); // ensure we don't divide by zero
    const qreal sinc = qSin(c);

    lon = centerLon + qAtan2( rx*sinc , ( c*qCos( centerLat )*qCos( c ) - ry*qSin( centerLat )*sinc  ) );

    while ( lon < -M_PI ) lon += 2 * M_PI;
    while ( lon >  M_PI ) lon -= 2 * M_PI;

    lat = qAsin( qCos(c)*qSin(centerLat) + (ry*sinc*qCos(centerLat))/c );

    if ( unit == GeoDataCoordinates::Degree ) {
        lon *= RAD2DEG;
        lat *= RAD2DEG;
    }

    return true;
}

}
