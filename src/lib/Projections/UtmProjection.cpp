//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007-2008 Inge Wallin  <ingwa@kde.org>
// Copyright 2008 Henry de Valence <hdevalence@gmail.com>
//

#include "UtmProjection.h"

#include "UtmProjectionHelper.h"

#include <math.h>

using namespace Marble;

static UtmProjectionHelper theHelper;

UtmProjection::UtmProjection()
{
    // These are the upper and lower bounds of the utm grid.
    // For some reason (proably norway, they made the other irregularities)
    // the northern one is extended 4 degrees.
    m_maxLat  = 84.0 * DEG2RAD;
    m_minLat  = -80.0 * DEG2RAD;
    m_traversableMaxLat = false;
    m_repeatX = true;
}

UtmProjection::~UtmProjection()
{
}


AbstractProjectionHelper* UtmProjection::helper()
{
    return &theHelper;
}

bool UtmProjection::screenCoordinates( const qreal lon, const qreal lat,
                                       const ViewportParams *params,
                                       int& x, int& y,
                                       CoordinateType coordType )
{
    if( lat > m_maxLat || lat < m_minLat ) {
        return false;
    }

    // maxw = max dist away from the slice (radians)
    const qreal  maxw = 3.0*DEG2RAD;
    // fullw = width of the slice (radians)
    const qreal fullw = 6.0*DEG2RAD;

    //we subtract 30 because there are 60 zones and they start at -pi
    //so to restore justice and balance to the universe etc we shift it over
    int lonz = (int) ( floor( ( lon+M_PI ) / ( fullw ) ) - 30 );
    qreal clon = maxw + ( ( fullw ) * num );

    int Rk = viewport->radius() * 0.9996;

    //x = Rk arctanh( sin lambda ) width of slice (pixels)
    int slicew = (int) ( ( Rk * atanh( sin( maxw ) ) ) * 2 );

    // Let (x, y) be the position on the screen of the placemark..
    x = (int)( Rk * atanh( cos(lat) * sin( lon - clon ) ) );
    y = (int)( Rk * atanh( tan(lat) / cos( lon - clon ) ) );

    //now we must adjust the coords from the slice
    x += slicew*lonz;

    return true;
}

bool UtmProjection::geoCoordinates( const int x, const int y,
                                    const ViewportParams *params,
                                    qreal& lon, qreal& lat,
                                    GeoDataCoordinates::Unit unit )
{

}


