//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2004-2007 Torsten Rahn <tackat@kde.org>
// Copyright 2007      Inge Wallin  <ingwa@kde.org>
// Copyright 2011      Bernhard Beschow <bbeschow@cs.tu-berlin.de>
// Copyright 2014      Dennis Nienhüser <nienhueser@kde.org>
//

//
// Quaternions provides a class that deals with quaternion operations.

// krazy:excludeall=dpointer,inline

#ifndef MARBLE_QUATERNION_H
#define MARBLE_QUATERNION_H

#include "marble_export.h"
#include <cmath>
#include <QtGlobal>

namespace Marble
{

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
    static Quaternion   fromSpherical(qreal lon, qreal lat);
    static Quaternion   fromEuler(qreal pitch, qreal yaw, qreal roll);

    static Quaternion slerp(const Quaternion &q1, const Quaternion &q2, qreal t);
    static Quaternion nlerp(const Quaternion &q1, const Quaternion &q2, qreal t);

    // Operators
    Quaternion  operator*(const Quaternion &q) const;
    Quaternion  operator+(const Quaternion &q) const;
    Quaternion  operator*(qreal factor) const;
    bool        operator==(const Quaternion &q) const;
    Quaternion& operator*=(const Quaternion &q);
    Quaternion& operator*=(qreal);

    void        getSpherical(qreal &lon, qreal &lat) const;

    void        normalize();

    qreal       length() const;

    Quaternion  inverse() const;
    Quaternion  log() const;
    Quaternion  exp() const;

    qreal       pitch() const;
    qreal       yaw() const;
    qreal       roll() const;


    void        rotateAroundAxis(const Quaternion &q);

    void        toMatrix(matrix &m) const;
    void        rotateAroundAxis(const matrix &m);

    // TODO: Better add accessors...
    xmmfloat    v;
};

}

#ifndef QT_NO_DEBUG_STREAM
MARBLE_EXPORT QDebug operator<<(QDebug, const Marble::Quaternion &);
#endif

#endif // MARBLE_QUATERNION_H
