//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2004-2007 Torsten Rahn <tackat@kde.org>"
// Copyright 2007      Inge Wallin  <ingwa@kde.org>"
//

//
// Quaternions provides a class that deals with quaternion operations.


#ifndef __MARBLE__QUATERNION_H
#define __MARBLE__QUATERNION_H

#include "marble_export.h"
#include <cmath>

enum
{
    Q_X = 0,
    Q_Y = 1,
    Q_Z = 2,
    Q_W = 3
};


typedef double    xmmfloat[4];
typedef xmmfloat  matrix[3];


class MARBLE_EXPORT Quaternion {
 public:
    Quaternion();
    Quaternion(double w, double x, double y, double z);
    /*!\brief used to generate Quaternion from longitude and latitude
     * 
     * \param lon longitude
     * \param lat latitude
     */
    Quaternion(double lon, double lat);

    // Operators
    Quaternion  operator*(const Quaternion &q) const;
    bool        operator==(const Quaternion &q) const;
    void        operator*=(const Quaternion &q);

    void        set(double w, double x, double y, double z) {
	v[Q_W] = w; v[Q_X] = x; v[Q_Y] = y; v[Q_Z] = z;
    }
    void        set(double &lon, double &lat) {
        v[Q_W] = 0.0;
        const double  cosLat = cos(lat);
        v[Q_X] = cosLat * sin(lon);
        v[Q_Y] = sin(lat);
        v[Q_Z] = cosLat * cos(lon);
    }
    
    void        getSpherical(double &lon, double &lat) const;

    void        normalize();

    Quaternion  inverse() const;

    void        createFromEuler(double pitch, double yaw, double roll);
    double      pitch() const;
    double      yaw() const;
    double      roll() const;


    void        display() const;

    void        rotateAroundAxis(const Quaternion &q);
    void        slerp(const Quaternion &q1, const Quaternion &q2, double t);

    void        scalar(double mult);

    void        toMatrix(matrix &m) const;
    void        rotateAroundAxis(const matrix &m);

    // TODO: Better add accessors...
    xmmfloat    v;
};

#endif // __MARBLE__QUATERNION_H
