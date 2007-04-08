//
// C++ Interface: texcolorizer
//
// Description: Quaternions 

// Quaternions provides a class that deals with quaternion operations.
//
// Author: Torsten Rahn <tackat@kde.org>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution

#ifndef QUATERNION_H
#define QUATERNION_H

enum
{
    Q_X = 0,
    Q_Y = 1,
    Q_Z = 2,
    Q_W = 3,
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

class QuaternionSSE : public Quaternion {
 public:
    void rotateAroundAxis(const Quaternion &q);
};

#endif
