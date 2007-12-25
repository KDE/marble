//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2006-2007 Torsten Rahn <tackat@kde.org>"
// Copyright 2007      Inge Wallin  <ingwa@kde.org>"
//

#include "Quaternion.h"

#include <cmath>

#include <QtCore/QString>
#include <QtCore/QDebug>


#define quatNorm (v[Q_W] * v[Q_W] + v[Q_X] * v[Q_X] + v[Q_Y] * v[Q_Y] + v[Q_Z] * v[Q_Z])


Quaternion::Quaternion()
{
//    like in libeigen we keep the quaternion uninitialized
//    set( 1.0, 0.0, 0.0, 0.0 );
}

Quaternion::Quaternion(double w, double x, double y, double z) 
{
    set( w, x, y, z );
}

Quaternion::Quaternion(double lon, double lat)
{
    v[Q_W] = 0.0;

    const double  cosLat = cos(lat);
    v[Q_X] = cosLat * sin(lon);
    v[Q_Y] = sin(lat);
    v[Q_Z] = cosLat * cos(lon);
}

void Quaternion::getSpherical(double &lon, double &lat) const 
{
    double  y = v[Q_Y];
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

void Quaternion::scalar(double mult)
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

void Quaternion::createFromEuler(double pitch, double yaw, double roll)
{
    double  cPhi, cThe, cPsi, sPhi, sThe, sPsi, 
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

    v[Q_W] = cPhi * cThecPsi + sPhi * sThesPsi;
    v[Q_X] = sPhi * cThecPsi - cPhi * sThesPsi;
    v[Q_Y] = cPhi * sThecPsi + sPhi * cThesPsi;
    v[Q_Z] = cPhi * cThesPsi - sPhi * sThecPsi;
}

double Quaternion::pitch() const
{
    return atan2( 2.0*(v[Q_X]*v[Q_W]-v[Q_Y]*v[Q_Z]),
                ( 1.0 - 2.0*(v[Q_X]*v[Q_X]+v[Q_Z]*v[Q_Z] ) ) );
}

double Quaternion::yaw() const
{
    return atan2( 2.0*(v[Q_Y]*v[Q_W]-v[Q_X]*v[Q_Z]),
                 (1.0 - 2.0*(v[Q_Y]*v[Q_Y]+v[Q_Z]*v[Q_Z] ) ) );
}

double Quaternion::roll() const
{
    return asin(2.0*(v[Q_X]*v[Q_Y]+v[Q_Z]*v[Q_W]));
}

void Quaternion::display() const
{
    QString quatdisplay = QString("Quaternion: w= %1, x= %2, y= %3, z= %4, |q|= %5" )
        .arg(v[Q_W]).arg(v[Q_X]).arg(v[Q_Y]).arg(v[Q_Z]).arg(quatNorm);

    qDebug() << quatdisplay;
}

void Quaternion::operator*=(const Quaternion &q)
{
    double x, y, z, w;

    w = v[Q_W] * q.v[Q_W] - v[Q_X] * q.v[Q_X] - v[Q_Y] * q.v[Q_Y] - v[Q_Z] * q.v[Q_Z];
    x = v[Q_W] * q.v[Q_X] + v[Q_X] * q.v[Q_W] + v[Q_Y] * q.v[Q_Z] - v[Q_Z] * q.v[Q_Y];
    y = v[Q_W] * q.v[Q_Y] - v[Q_X] * q.v[Q_Z] + v[Q_Y] * q.v[Q_W] + v[Q_Z] * q.v[Q_X];
    z = v[Q_W] * q.v[Q_Z] + v[Q_X] * q.v[Q_Y] - v[Q_Y] * q.v[Q_X] + v[Q_Z] * q.v[Q_W];

    set( w, x, y, z );
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
    double  w, x, y, z;

    w = v[Q_W] * q.v[Q_W] - v[Q_X] * q.v[Q_X] - v[Q_Y] * q.v[Q_Y] - v[Q_Z] * q.v[Q_Z];
    x = v[Q_W] * q.v[Q_X] + v[Q_X] * q.v[Q_W] + v[Q_Y] * q.v[Q_Z] - v[Q_Z] * q.v[Q_Y];
    y = v[Q_W] * q.v[Q_Y] - v[Q_X] * q.v[Q_Z] + v[Q_Y] * q.v[Q_W] + v[Q_Z] * q.v[Q_X];
    z = v[Q_W] * q.v[Q_Z] + v[Q_X] * q.v[Q_Y] - v[Q_Y] * q.v[Q_X] + v[Q_Z] * q.v[Q_W];
    return Quaternion( w, x, y, z );
}

void Quaternion::rotateAroundAxis(const Quaternion &q)
{
    double w, x, y, z;

    w = + v[Q_X] * q.v[Q_X] + v[Q_Y] * q.v[Q_Y] + v[Q_Z] * q.v[Q_Z];
    x = + v[Q_X] * q.v[Q_W] - v[Q_Y] * q.v[Q_Z] + v[Q_Z] * q.v[Q_Y];
    y = + v[Q_X] * q.v[Q_Z] + v[Q_Y] * q.v[Q_W] - v[Q_Z] * q.v[Q_X];
    z = - v[Q_X] * q.v[Q_Y] + v[Q_Y] * q.v[Q_X] + v[Q_Z] * q.v[Q_W];

    v[Q_W] = q.v[Q_W] * w - q.v[Q_X] * x - q.v[Q_Y] * y - q.v[Q_Z] * z;
    v[Q_X] = q.v[Q_W] * x + q.v[Q_X] * w + q.v[Q_Y] * z - q.v[Q_Z] * y;
    v[Q_Y] = q.v[Q_W] * y - q.v[Q_X] * z + q.v[Q_Y] * w + q.v[Q_Z] * x;
    v[Q_Z] = q.v[Q_W] * z + q.v[Q_X] * y - q.v[Q_Y] * x + q.v[Q_Z] * w;
}

void Quaternion::slerp(const Quaternion q1, const Quaternion q2, double t)
{
    double p1, p2;

    double cosAlpha = ( q1.v[Q_X]*q2.v[Q_X] + q1.v[Q_Y]*q2.v[Q_Y]
		       + q1.v[Q_Z]*q2.v[Q_Z] + q1.v[Q_W]*q2.v[Q_W] );
    double alpha    = acos(cosAlpha);
    double sinAlpha = sin(alpha);

    if( sinAlpha > 0.0 ) {
        p1 = sin( (1.0-t)*alpha ) / sinAlpha;
        p2 = sin( t*alpha ) / sinAlpha;
    } else {
        // both Quaternions are equal
        p1 = 1.0;
        p2 = 0.0;
    }

    v[Q_X] = p1*q1.v[Q_X] + p2*q2.v[Q_X];
    v[Q_Y] = p1*q1.v[Q_Y] + p2*q2.v[Q_Y];
    v[Q_Z] = p1*q1.v[Q_Z] + p2*q2.v[Q_Z];
    v[Q_W] = p1*q1.v[Q_W] + p2*q2.v[Q_W];
}

void Quaternion::toMatrix(matrix &m) const
{

    double xy = v[Q_X] * v[Q_Y], xz = v[Q_X] * v[Q_Z];
    double yy = v[Q_Y] * v[Q_Y], yw = v[Q_Y] * v[Q_W];
    double zw = v[Q_Z] * v[Q_W], zz = v[Q_Z] * v[Q_Z];

    m[0][0] = 1.0 - 2.0 * (yy + zz);
    m[0][1] = 2.0 * (xy + zw);
    m[0][2] = 2.0 * (xz - yw);
    m[0][3] = 0.0;

    double xx = v[Q_X] * v[Q_X], xw = v[Q_X] * v[Q_W], yz = v[Q_Y] * v[Q_Z];

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
    double x, y, z;

    x =  m[0][0] * v[Q_X] + m[1][0] * v[Q_Y] + m[2][0] * v[Q_Z];
    y =  m[0][1] * v[Q_X] + m[1][1] * v[Q_Y] + m[2][1] * v[Q_Z];
    z =  m[0][2] * v[Q_X] + m[1][2] * v[Q_Y] + m[2][2] * v[Q_Z];

    v[Q_W] = 1.0; v[Q_X] = x; v[Q_Y] = y; v[Q_Z] = z;
}
