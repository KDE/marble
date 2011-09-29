//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2006-2007 Torsten Rahn <tackat@kde.org>
// Copyright 2007      Inge Wallin  <ingwa@kde.org>
//

#include "Quaternion.h"

#include <cmath>
using namespace std;

#include <QtCore/QString>
#include "MarbleDebug.h"


#define quatNorm (v[Q_W] * v[Q_W] + v[Q_X] * v[Q_X] + v[Q_Y] * v[Q_Y] + v[Q_Z] * v[Q_Z])


using namespace Marble;

Quaternion::Quaternion()
{
//    like in libeigen we keep the quaternion uninitialized
//    set( 1.0, 0.0, 0.0, 0.0 );
}

Quaternion::Quaternion(qreal w, qreal x, qreal y, qreal z)
{
    v[Q_W] = w;
    v[Q_X] = x;
    v[Q_Y] = y;
    v[Q_Z] = z;
}

Quaternion Quaternion::fromSpherical(qreal lon, qreal lat)
{
    const qreal  cosLat = cos(lat);

    return Quaternion( 0.0, cosLat * sin(lon), sin(lat), cosLat * cos(lon) );
}

void Quaternion::getSpherical(qreal &lon, qreal &lat) const 
{
    qreal  y = v[Q_Y];
    if ( y > 1.0 )
        y = 1.0;
    else if ( y < -1.0 )
        y = -1.0;

    lat = asin( y );

    if(v[Q_X] * v[Q_X] + v[Q_Z] * v[Q_Z] > 0.00005) 
        lon = atan2(v[Q_X], v[Q_Z]);
    else
        lon = 0.0;
}

void Quaternion::normalize() 
{
    scalar( 1.0 / sqrt(quatNorm) );
}

void Quaternion::scalar(qreal mult)
{
    v[Q_W] *= mult;
    v[Q_X] *= mult;
    v[Q_Y] *= mult;
    v[Q_Z] *= mult;
}

Quaternion Quaternion::inverse() const
{
    Quaternion  inverse( v[Q_W], -v[Q_X], -v[Q_Y], -v[Q_Z] );
    inverse.normalize();

    return inverse;
}

Quaternion Quaternion::fromEuler(qreal pitch, qreal yaw, qreal roll)
{
    qreal  cPhi, cThe, cPsi, sPhi, sThe, sPsi, 
            cThecPsi, sThesPsi, sThecPsi, cThesPsi;

    pitch *= 0.5;
    yaw   *= 0.5;
    roll  *= 0.5;

    cPhi = cos(pitch); // also: "heading"
    cThe = cos(yaw);   // also: "attitude" 
    cPsi = cos(roll);  // also: "bank"

    sPhi = sin(pitch);
    sThe = sin(yaw);
    sPsi = sin(roll);

    cThecPsi = cThe * cPsi;
    sThesPsi = sThe * sPsi;
    sThecPsi = sThe * cPsi;
    cThesPsi = cThe * sPsi;

    const qreal w = cPhi * cThecPsi + sPhi * sThesPsi;
    const qreal x = sPhi * cThecPsi - cPhi * sThesPsi;
    const qreal y = cPhi * sThecPsi + sPhi * cThesPsi;
    const qreal z = cPhi * cThesPsi - sPhi * sThecPsi;

    return Quaternion( w, x, y, z );
}

qreal Quaternion::pitch() const // "heading", phi
{
    return atan2( 2.0*(v[Q_X]*v[Q_W]-v[Q_Y]*v[Q_Z]),
                ( 1.0 - 2.0*(v[Q_X]*v[Q_X]+v[Q_Z]*v[Q_Z] ) ) );
}

qreal Quaternion::yaw() const // "attitude", theta
{
    return atan2( 2.0*(v[Q_Y]*v[Q_W]-v[Q_X]*v[Q_Z]),
                 (1.0 - 2.0*(v[Q_Y]*v[Q_Y]+v[Q_Z]*v[Q_Z] ) ) );
}

qreal Quaternion::roll() const // "bank", psi 
{
    return asin(2.0*(v[Q_X]*v[Q_Y]+v[Q_Z]*v[Q_W]));
}

void Quaternion::display() const
{
    QString quatdisplay = QString("Quaternion: w= %1, x= %2, y= %3, z= %4, |q|= %5" )
        .arg(v[Q_W]).arg(v[Q_X]).arg(v[Q_Y]).arg(v[Q_Z]).arg(quatNorm);

    mDebug() << quatdisplay;
}

Quaternion& Quaternion::operator*=(const Quaternion &q)
{
    (*this) = (*this) * q;

    return *this;
}

bool Quaternion::operator==(const Quaternion &q) const
{

    return ( v[Q_W] == q.v[Q_W]
         && v[Q_X] == q.v[Q_X]
         && v[Q_Y] == q.v[Q_Y]
         && v[Q_Z] == q.v[Q_Z] );
}

Quaternion Quaternion::operator*(const Quaternion &q) const
{
    qreal  w, x, y, z;

    w = v[Q_W] * q.v[Q_W] - v[Q_X] * q.v[Q_X] - v[Q_Y] * q.v[Q_Y] - v[Q_Z] * q.v[Q_Z];
    x = v[Q_W] * q.v[Q_X] + v[Q_X] * q.v[Q_W] + v[Q_Y] * q.v[Q_Z] - v[Q_Z] * q.v[Q_Y];
    y = v[Q_W] * q.v[Q_Y] - v[Q_X] * q.v[Q_Z] + v[Q_Y] * q.v[Q_W] + v[Q_Z] * q.v[Q_X];
    z = v[Q_W] * q.v[Q_Z] + v[Q_X] * q.v[Q_Y] - v[Q_Y] * q.v[Q_X] + v[Q_Z] * q.v[Q_W];
    return Quaternion( w, x, y, z );
}

void Quaternion::rotateAroundAxis(const Quaternion &q)
{
    qreal w, x, y, z;

    w = + v[Q_X] * q.v[Q_X] + v[Q_Y] * q.v[Q_Y] + v[Q_Z] * q.v[Q_Z];
    x = + v[Q_X] * q.v[Q_W] - v[Q_Y] * q.v[Q_Z] + v[Q_Z] * q.v[Q_Y];
    y = + v[Q_X] * q.v[Q_Z] + v[Q_Y] * q.v[Q_W] - v[Q_Z] * q.v[Q_X];
    z = - v[Q_X] * q.v[Q_Y] + v[Q_Y] * q.v[Q_X] + v[Q_Z] * q.v[Q_W];

    (*this) = q * Quaternion( w, x, y, z );
}

void Quaternion::slerp( const Quaternion &q1, const Quaternion &q2, 
                        qreal t)
{
    qreal  p1;
    qreal  p2;

    // Let alpha be the angle between the two quaternions.
    qreal  cosAlpha = ( q1.v[Q_X] * q2.v[Q_X]
                         + q1.v[Q_Y] * q2.v[Q_Y]
                         + q1.v[Q_Z] * q2.v[Q_Z]
                         + q1.v[Q_W] * q2.v[Q_W] );
    qreal  alpha    = acos( cosAlpha );
    qreal  sinAlpha = sin( alpha );

    if ( sinAlpha > 0.0 ) {
        p1 = sin( ( 1.0 - t ) * alpha ) / sinAlpha;
        p2 = sin( t           * alpha ) / sinAlpha;
    } else {
        // both Quaternions are equal
        p1 = 1.0;
        p2 = 0.0;
    }

    v[Q_X] = p1 * q1.v[Q_X] + p2 * q2.v[Q_X];
    v[Q_Y] = p1 * q1.v[Q_Y] + p2 * q2.v[Q_Y];
    v[Q_Z] = p1 * q1.v[Q_Z] + p2 * q2.v[Q_Z];
    v[Q_W] = p1 * q1.v[Q_W] + p2 * q2.v[Q_W];
}

void Quaternion::nlerp( const Quaternion &q1, const Quaternion &q2, 
                        qreal t)
{
    qreal  p1 = 1.0 - t;
 
    v[Q_X] = p1 * q1.v[Q_X] + t * q2.v[Q_X];
    v[Q_Y] = p1 * q1.v[Q_Y] + t * q2.v[Q_Y];
    v[Q_Z] = p1 * q1.v[Q_Z] + t * q2.v[Q_Z];
    v[Q_W] = p1 * q1.v[Q_W] + t * q2.v[Q_W];

    normalize();
}

void Quaternion::toMatrix(matrix &m) const
{

    qreal xy = v[Q_X] * v[Q_Y], xz = v[Q_X] * v[Q_Z];
    qreal yy = v[Q_Y] * v[Q_Y], yw = v[Q_Y] * v[Q_W];
    qreal zw = v[Q_Z] * v[Q_W], zz = v[Q_Z] * v[Q_Z];

    m[0][0] = 1.0 - 2.0 * (yy + zz);
    m[0][1] = 2.0 * (xy + zw);
    m[0][2] = 2.0 * (xz - yw);
    m[0][3] = 0.0;

    qreal xx = v[Q_X] * v[Q_X], xw = v[Q_X] * v[Q_W], yz = v[Q_Y] * v[Q_Z];

    m[1][0] = 2.0 * (xy - zw);
    m[1][1] = 1.0 - 2.0 * (xx + zz);
    m[1][2] = 2.0 * (yz + xw);
    m[1][3] = 0.0;

    m[2][0] = 2.0 * (xz + yw);
    m[2][1] = 2.0 * (yz - xw);
    m[2][2] = 1.0 - 2.0 * (xx + yy);
    m[2][3] = 0.0;
}

void Quaternion::rotateAroundAxis(const matrix &m)
{
    qreal x, y, z;

    x =  m[0][0] * v[Q_X] + m[1][0] * v[Q_Y] + m[2][0] * v[Q_Z];
    y =  m[0][1] * v[Q_X] + m[1][1] * v[Q_Y] + m[2][1] * v[Q_Z];
    z =  m[0][2] * v[Q_X] + m[1][2] * v[Q_Y] + m[2][2] * v[Q_Z];

    v[Q_W] = 1.0; v[Q_X] = x; v[Q_Y] = y; v[Q_Z] = z;
}
