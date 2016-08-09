//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007      Torsten Rahn <tackat@kde.org>
// Copyright 2007      Inge Wallin  <ingwa@kde.org>
//

#ifndef MARBLE_MARBLEMATH_H
#define MARBLE_MARBLEMATH_H

#include <QtGlobal>

#include "GeoDataCoordinates.h"
#include <math.h>


namespace
{
    const qreal a1 = 1.0/6.0; 
    const qreal a2 = 1.0/24.0; 
    const qreal a3 = 61.0/5040; 
    const qreal a4 = 277.0/72576.0;  
    const qreal a5 = 50521.0/39916800.0; 
    const qreal a6 = 41581.0/95800320.0; 
    const qreal a7 = 199360981.0/1307674368000.0; 
    const qreal a8 = 228135437.0/4184557977600.0; 
    const qreal a9 = 2404879675441.0/121645100408832000.0; 
    const qreal a10 = 14814847529501.0/2043637686868377600.0; 
    const qreal a11 = 69348874393137901.0/25852016738884976640000.0; 
    const qreal a12 = 238685140977801337.0/238634000666630553600000.0; 
    const qreal a13 = 4087072509293123892361.0/10888869450418352160768000000.0;
    const qreal a14 = 454540704683713199807.0/3209350995912777478963200000.0;
    const qreal a15 = 441543893249023104553682821.0/8222838654177922817725562880000000.0;
    const qreal a16 = 2088463430347521052196056349.0/102156677868375135241390522368000000.0;
}

namespace Marble
{

/**
 * @brief This method calculates the shortest distance between two points on a sphere.
 * @brief See: http://en.wikipedia.org/wiki/Great-circle_distance
 * @param lon1 longitude of first point in radians
 * @param lat1 latitude of first point in radians
 * @param lon2 longitude of second point in radians
 * @param lat2 latitude of second point in radians
 */
inline qreal distanceSphere( qreal lon1, qreal lat1, qreal lon2, qreal lat2 ) {

    qreal h1 = sin( 0.5 * ( lat2 - lat1 ) );
    qreal h2 = sin( 0.5 * ( lon2 - lon1 ) );
    qreal d = h1 * h1 + cos( lat1 ) * cos( lat2 ) * h2 * h2;

    return 2.0 * atan2( sqrt( d ), sqrt( 1.0 - d ) );
}


/**
 * @brief This method calculates the shortest distance between two points on a sphere.
 * @brief See: http://en.wikipedia.org/wiki/Great-circle_distance
 */
inline qreal distanceSphere( const GeoDataCoordinates& coords1, const GeoDataCoordinates& coords2 ) {

    qreal lon1, lat1;
    coords1.geoCoordinates( lon1, lat1 );
    qreal lon2, lat2;
    coords2.geoCoordinates( lon2, lat2 );

    // FIXME: Take the altitude into account!

    return distanceSphere( lon1, lat1, lon2, lat2 );
}


/**
 * @brief This method roughly calculates the shortest distance between two points on a sphere.
 * @brief It's probably faster than distanceSphere(...) but for 7 significant digits only has
 * @brief an accuracy of about 1 arcmin.
 * @brief See: http://en.wikipedia.org/wiki/Great-circle_distance
 */
inline qreal distanceSphereApprox( qreal lon1, qreal lat1, qreal lon2, qreal lat2 ) {
    return acos( sin( lat1 ) * sin( lat2 ) + cos( lat1 ) * cos( lat2 ) * cos( lon1 - lon2 ) );
}


/**
 * @brief This method is a fast Mac Laurin power series approximation of the 
 * @brief inverse Gudermannian. The inverse Gudermannian gives the vertical 
 * @brief position y in the Mercator projection in terms of the latitude.
 * @brief See: http://en.wikipedia.org/wiki/Mercator_projection
 */
inline qreal gdInv( qreal x ) {
        const qreal x2 = x * x;
        return x 
            + x * x2 * (  a1
            + x2 * ( a2  + x2 * ( a3  + x2 * ( a4  + x2 * ( a5
            + x2 * ( a6  + x2 * ( a7  + x2 * ( a8  + x2 * ( a9
            + x2 * ( a10 + x2 * ( a11 + x2 * ( a12 + x2 * ( a13
            + x2 * ( a14 + x2 * ( a15 + x2 * ( a16 ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) );
}

}

/**
 * @brief This method is a fast Mac Laurin power series approximation of the
 *        Gudermannian. The Gudermannian gives the latitude
 *        in the Mercator projection in terms of the vertical position y.
 *        See: http://en.wikipedia.org/wiki/Mercator_projection
 */
inline qreal gd( qreal x ) {

    /*
    const qreal x2 = x * x;
    return x
         - x * x2 * (  a1
         - x2 * ( a2  - x2 * ( a3  - x2 * ( a4  - x2 * ( a5
         - x2 * ( a6  - x2 * ( a7  - x2 * ( a8  - x2 * ( a9
         - x2 * ( a10 - x2 * ( a11 - x2 * ( a12 - x2 * ( a13
         - x2 * ( a14 - x2 * ( a15 - x2 * ( a16 ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) );
    */

    return atan ( sinh ( x ) );
}

#endif
