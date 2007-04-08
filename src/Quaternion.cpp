#include <cmath>

#include <QtCore/QString>
#include <QtCore/QDebug>

#include "Quaternion.h"


#define quatNorm (v[Q_W] * v[Q_W] + v[Q_X] * v[Q_X] + v[Q_Y] * v[Q_Y] + v[Q_Z] * v[Q_Z])


Quaternion::Quaternion()
{
    set( 1.0f, 0.0f, 0.0f, 0.0f );
}

Quaternion::Quaternion(float w, float x, float y, float z) 
{
    set( w, x, y, z );
}

Quaternion::Quaternion(float alpha, float beta)
{
    v[Q_W] = 0.0f;

    const float  cosBeta = cosf(beta);
    v[Q_X] = -cosBeta * sinf(alpha);
    v[Q_Y] = -sinf(beta);
    v[Q_Z] = cosBeta * cosf(alpha);
}


void Quaternion::normalize() 
{
    scalar( 1.0f / sqrtf(quatNorm) );
}

void Quaternion::scalar(float mult)
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

void Quaternion::createFromEuler(float pitch, float yaw, float roll)
{
    float cX, cY, cZ, sX, sY, sZ, cYcZ, sYsZ, sYcZ, cYsZ;

    pitch *= 0.5f;
    yaw   *= 0.5f;
    roll  *= 0.5f;

    cX = cosf(pitch);
    cY = cosf(yaw);
    cZ = cosf(roll);

    sX = sinf(pitch);
    sY = sinf(yaw);
    sZ = sinf(roll);

    cYcZ = cY * cZ;
    sYsZ = sY * sZ;
    sYcZ = sY * cZ;
    cYsZ = cY * sZ;

    v[Q_W] = cX * cYcZ + sX * sYsZ;
    v[Q_X] = sX * cYcZ - cX * sYsZ;
    v[Q_Y] = cX * sYcZ + sX * cYsZ;
    v[Q_Z] = cX * cYsZ - sX * sYcZ;
}


void Quaternion::display() const
{
    QString quatdisplay = QString("Quaternion: w= %1, x= %2, y= %3, z= %4, |q|= %5" )
	.arg(v[Q_W]).arg(v[Q_X]).arg(v[Q_Y]).arg(v[Q_Z]).arg(quatNorm);

    qDebug(quatdisplay.toLatin1());
}

void Quaternion::operator*=(const Quaternion &q)
{
    float x, y, z, w;

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
    float  w, x, y, z;

    w = v[Q_W] * q.v[Q_W] - v[Q_X] * q.v[Q_X] - v[Q_Y] * q.v[Q_Y] - v[Q_Z] * q.v[Q_Z];
    x = v[Q_W] * q.v[Q_X] + v[Q_X] * q.v[Q_W] + v[Q_Y] * q.v[Q_Z] - v[Q_Z] * q.v[Q_Y];
    y = v[Q_W] * q.v[Q_Y] - v[Q_X] * q.v[Q_Z] + v[Q_Y] * q.v[Q_W] + v[Q_Z] * q.v[Q_X];
    z = v[Q_W] * q.v[Q_Z] + v[Q_X] * q.v[Q_Y] - v[Q_Y] * q.v[Q_X] + v[Q_Z] * q.v[Q_W];
    return Quaternion( w, x, y, z );
}

void Quaternion::rotateAroundAxis(const Quaternion &q)
{
    float w, x, y, z;

    w = + v[Q_X] * q.v[Q_X] + v[Q_Y] * q.v[Q_Y] + v[Q_Z] * q.v[Q_Z];
    x = + v[Q_X] * q.v[Q_W] - v[Q_Y] * q.v[Q_Z] + v[Q_Z] * q.v[Q_Y];
    y = + v[Q_X] * q.v[Q_Z] + v[Q_Y] * q.v[Q_W] - v[Q_Z] * q.v[Q_X];
    z = - v[Q_X] * q.v[Q_Y] + v[Q_Y] * q.v[Q_X] + v[Q_Z] * q.v[Q_W];

    v[Q_W] = q.v[Q_W] * w - q.v[Q_X] * x - q.v[Q_Y] * y - q.v[Q_Z] * z;
    v[Q_X] = q.v[Q_W] * x + q.v[Q_X] * w + q.v[Q_Y] * z - q.v[Q_Z] * y;
    v[Q_Y] = q.v[Q_W] * y - q.v[Q_X] * z + q.v[Q_Y] * w + q.v[Q_Z] * x;
    v[Q_Z] = q.v[Q_W] * z + q.v[Q_X] * y - q.v[Q_Y] * x + q.v[Q_Z] * w;
}

void Quaternion::slerp(const Quaternion q1, const Quaternion q2, float t)
{
    float p1, p2;

    float cosAlpha = ( q1.v[Q_X]*q2.v[Q_X] + q1.v[Q_Y]*q2.v[Q_Y]
		       + q1.v[Q_Z]*q2.v[Q_Z] + q1.v[Q_W]*q2.v[Q_W] );
    float alpha    = acosf(cosAlpha);
    float sinAlpha = sinf(alpha);

    if( sinAlpha > 0.0f ) {
	p1 = sinf( (1.0f-t)*alpha ) / sinAlpha;
	p2 = sinf( t*alpha ) / sinAlpha;
    } else {
	// both Quaternions are equal
	p1 = 1.0f;
	p2 = 0.0f;
    }

    v[Q_X] = p1*q1.v[Q_X] + p2*q2.v[Q_X];
    v[Q_Y] = p1*q1.v[Q_Y] + p2*q2.v[Q_Y];
    v[Q_Z] = p1*q1.v[Q_Z] + p2*q2.v[Q_Z];
    v[Q_W] = p1*q1.v[Q_W] + p2*q2.v[Q_W];
}

void Quaternion::getSpherical(float &alpha, float &beta) const 
{
    float  y = v[Q_Y];
    if ( y > 1.0f )
	y = 1.0f;
    else if ( y < -1.0f )
	y = -1.0f;
    beta = -asinf( y );

    if(v[Q_X] * v[Q_X] + v[Q_Z] * v[Q_Z] > 0.00005f) 
	alpha = -atan2f(v[Q_X], v[Q_Z]);
    else
	alpha = 0.0f;
}

void Quaternion::toMatrix(matrix &m)
{

    float xy = v[Q_X] * v[Q_Y], xz = v[Q_X] * v[Q_Z];
    float yy = v[Q_Y] * v[Q_Y], yw = v[Q_Y] * v[Q_W];
    float zw = v[Q_Z] * v[Q_W], zz = v[Q_Z] * v[Q_Z];

    m[0][0] = 1.0f - 2.0f * (yy + zz);
    m[0][1] = 2.0f * (xy + zw);
    m[0][2] = 2.0f * (xz - yw);
    m[0][3] = 0.0f;

    float xx = v[Q_X] * v[Q_X], xw = v[Q_X] * v[Q_W], yz = v[Q_Y] * v[Q_Z];

    m[1][0] = 2.0f * (xy - zw);
    m[1][1] = 1.0f - 2.0f * (xx + zz);
    m[1][2] = 2.0f * (yz + xw);
    m[1][3] = 0.0f;

    m[2][0] = 2.0f * (xz + yw);
    m[2][1] = 2.0f * (yz - xw);
    m[2][2] = 1.0f - 2.0f * (xx + yy);
    m[2][3] = 0.0f;
}

void Quaternion::rotateAroundAxis(const matrix &m)
{
    float x, y, z;

    x =  m[0][0] * v[Q_X] + m[1][0] * v[Q_Y] + m[2][0] * v[Q_Z];
    y =  m[0][1] * v[Q_X] + m[1][1] * v[Q_Y] + m[2][1] * v[Q_Z];
    z =  m[0][2] * v[Q_X] + m[1][2] * v[Q_Y] + m[2][2] * v[Q_Z];

    set( 1.0f, x, y, z );
}
