//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007        Inge Wallin   <ingwa@kde.org>
// Copyright 2007-2014   Torsten Rahn  <rahn@kde.org>
// Copyright 2009        Patrick Spendrin <ps_ml@gmx.de>
// Copyright 2012        Cezar Mocan   <mocancezar@gmail.com>
//

// Local
#include "SphericalProjection.h"
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

#define SAFE_DISTANCE

namespace Marble
{

class SphericalProjectionPrivate : public AzimuthalProjectionPrivate
{
  public:

    explicit SphericalProjectionPrivate( SphericalProjection * parent );

    Q_DECLARE_PUBLIC( SphericalProjection )
};

SphericalProjection::SphericalProjection()
    : AzimuthalProjection( new SphericalProjectionPrivate( this ) )
{
    setMinLat( minValidLat() );
    setMaxLat( maxValidLat() );
}

SphericalProjection::SphericalProjection( SphericalProjectionPrivate *dd )
        : AzimuthalProjection( dd )
{
    setMinLat( minValidLat() );
    setMaxLat( maxValidLat() );
}

SphericalProjection::~SphericalProjection()
{
}

SphericalProjectionPrivate::SphericalProjectionPrivate( SphericalProjection * parent )
        : AzimuthalProjectionPrivate( parent )
{
}

QString SphericalProjection::name() const
{
    return QObject::tr( "Globe" );
}

QString SphericalProjection::description() const
{
    return QObject::tr( "<p><b>Orthographic Projection</b> (\"orthogonal\")</p><p>Applications: A perspective projection that is used to display the hemisphere of a globe as it appears from outer space.</p>" );
}

QIcon SphericalProjection::icon() const
{
    return QIcon(QStringLiteral(":/icons/map-globe.png"));
}

bool SphericalProjection::screenCoordinates( const GeoDataCoordinates &coordinates, 
                                             const ViewportParams *viewport,
                                             qreal &x, qreal &y, bool &globeHidesPoint ) const
{
    const qreal altitude = coordinates.altitude();
    const qreal absoluteAltitude = altitude + EARTH_RADIUS;
    Quaternion  qpos             = coordinates.quaternion();

    qpos.rotateAroundAxis( viewport->planetAxisMatrix() );

    const qreal radius = viewport->radius();
    const qreal pixelAltitude = (radius / EARTH_RADIUS * absoluteAltitude);
    if (altitude < 10000) {
        // Skip placemarks at the other side of the earth.
        if ( qpos.v[Q_Z] < 0 ) {
            globeHidesPoint = true;
            return false;
        }
    }
    else {
        qreal  earthCenteredX = pixelAltitude * qpos.v[Q_X];
        qreal  earthCenteredY = pixelAltitude * qpos.v[Q_Y];

        // Don't draw high placemarks (e.g. satellites) that aren't visible.
        if ( qpos.v[Q_Z] < 0
             && ( ( earthCenteredX * earthCenteredX
                    + earthCenteredY * earthCenteredY )
                  < radius * radius ) ) {
            globeHidesPoint = true;
            return false;
        }
    }

    const qreal width = viewport->width();
    const qreal height = viewport->height();

    // Let (x, y) be the position on the screen of the placemark..
    x = (width  / 2 + pixelAltitude * qpos.v[Q_X]);
    y = (height / 2 - pixelAltitude * qpos.v[Q_Y]);

    // Skip placemarks that are outside the screen area
    if (x < 0 || x >= width || y < 0 || y >= height) {
        globeHidesPoint = false;
        return false;
    }

    globeHidesPoint = false;
    return true;
}

bool SphericalProjection::screenCoordinates( const GeoDataCoordinates &coordinates,
                                             const ViewportParams *viewport,
                                             qreal *x, qreal &y,
                                             int &pointRepeatNum,
                                             const QSizeF& size,
                                             bool &globeHidesPoint ) const
{
    pointRepeatNum = 0;
    bool visible = screenCoordinates( coordinates, viewport, *x, y, globeHidesPoint );

    // Skip placemarks that are outside the screen area
    if ( *x + size.width() / 2.0 < 0.0 || *x >= viewport->width() + size.width() / 2.0 
         || y + size.height() / 2.0 < 0.0 || y >= viewport->height() + size.height() / 2.0 )
    {
        globeHidesPoint = false;
        return false;
    }

    // This projection doesn't have any repetitions, 
    // so the number of screen points referring to the geopoint is one.
    pointRepeatNum = 1;
    return visible;
}


bool SphericalProjection::geoCoordinates( const int x, const int y,
                                          const ViewportParams *viewport,
                                          qreal& lon, qreal& lat,
                                          GeoDataCoordinates::Unit unit ) const
{
    const qreal  inverseRadius = 1.0 / (qreal)(viewport->radius());

    const qreal qx = +(qreal)( x - viewport->width()  / 2 ) * inverseRadius;
    const qreal qy = -(qreal)( y - viewport->height() / 2 ) * inverseRadius;

    if ( 1 <= qx * qx + qy * qy ) {
        return false;
    }

    const qreal qz = sqrt( 1 - qx * qx - qy * qy );

    Quaternion  qpos( 0.0, qx, qy, qz );
    qpos.rotateAroundAxis( viewport->planetAxis() );
    qpos.getSpherical( lon, lat );

    if ( unit == GeoDataCoordinates::Degree ) {
        lon *= RAD2DEG;
        lat *= RAD2DEG;
    }

    return true;
}

}
