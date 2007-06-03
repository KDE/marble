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

enum
{
    Q_X = 0,
    Q_Y = 1,
    Q_Z = 2,
    Q_W = 3
};


typedef float     xmmfloat[4];
typedef xmmfloat  matrix[3];


class Quaternion {
 public:
    Quaternion();
    Quaternion(float w, float x, float y, float z);
    Quaternion(float alpha, float beta);
    virtual ~Quaternion(){ }

    // Operators
    Quaternion  operator*(const Quaternion &q) const;
    bool        operator==(const Quaternion &q) const;
    void        operator*=(const Quaternion &q);

    void        set(float w, float x, float y, float z) {
	v[Q_W] = w; v[Q_X] = x; v[Q_Y] = y; v[Q_Z] = z;
    }

    void        normalize();

    Quaternion  inverse() const;

    void        createFromEuler(float pitch, float yaw, float roll);
    float       pitch() const;
    float       yaw() const;


    void        display() const;

    virtual void rotateAroundAxis(const Quaternion &q);
    void        slerp(const Quaternion q1, const Quaternion q2, float t);

    void        getSpherical(float &alpha, float &beta) const; // Geo: lon, lat

    void        scalar(float mult);

    void        toMatrix(matrix &m);
    void        rotateAroundAxis(const matrix &m);

    // TODO: Better add accessors...
    xmmfloat    v;
};

#endif // __MARBLE__QUATERNION_H
