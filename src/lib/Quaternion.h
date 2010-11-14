//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2004-2007 Torsten Rahn <tackat@kde.org>
// Copyright 2007      Inge Wallin  <ingwa@kde.org>
//

//
// Quaternions provides a class that deals with quaternion operations.

// krazy:excludeall=dpointer,inline

#ifndef MARBLE_QUATERNION_H
#define MARBLE_QUATERNION_H

#include "marble_export.h"
#include <cmath>

namespace Marble
{

using std::cos;
using std::sin;

enum
{
    Q_X = 0,
    Q_Y = 1,
    Q_Z = 2,
    Q_W = 3
};


typedef qreal    xmmfloat[4];
typedef xmmfloat  matrix[3];


class MARBLE_EXPORT Quaternion
{
 public:
    Quaternion();
    Quaternion(qreal w, qreal x, qreal y, qreal z);
    /*!\brief used to generate Quaternion from longitude and latitude
     * 
     * \param lon longitude
     * \param lat latitude
     */
    Quaternion(qreal lon, qreal lat);

    // Operators
    Quaternion  operator*(const Quaternion &q) const;
    bool        operator==(const Quaternion &q) const;
    Quaternion& operator*=(const Quaternion &q);

    void        set(qreal w, qreal x, qreal y, qreal z);
    void        set(qreal lon, qreal lat);

    void        getSpherical(qreal &lon, qreal &lat) const;

    void        normalize();

    Quaternion  inverse() const;

    void        createFromEuler(qreal pitch, qreal yaw, qreal roll);
    qreal       pitch() const;
    qreal       yaw() const;
    qreal       roll() const;


    void        display() const;

    void        rotateAroundAxis(const Quaternion &q);

    void        slerp(const Quaternion &q1, const Quaternion &q2, qreal t);
    void        nlerp(const Quaternion &q1, const Quaternion &q2, qreal t);

    void        scalar(qreal mult);

    void        toMatrix(matrix &m) const;
    void        rotateAroundAxis(const matrix &m);

    // TODO: Better add accessors...
    xmmfloat    v;
};

inline void Quaternion::set( qreal w, qreal x, qreal y, qreal z )
{
    v[Q_W] = w;
    v[Q_X] = x;
    v[Q_Y] = y;
    v[Q_Z] = z;
}

inline void Quaternion::set( qreal lon, qreal lat )
{
    v[Q_W] = 0.0;
    const qreal cosLat = cos( lat );
    v[Q_X] = cosLat * sin( lon );
    v[Q_Y] = sin( lat );
    v[Q_Z] = cosLat * cos( lon );
}

}

#endif // MARBLE_QUATERNION_H
