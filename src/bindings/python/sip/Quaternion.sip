//
// Copyright 2008 Simon Edwards <simon@simonzone.com>
// 
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either 
// version 2.1 of the License, or (at your option) any later version.
// 
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public 
// License along with this library.  If not, see <http://www.gnu.org/licenses/>.
//

namespace Marble
{

enum
{
    Q_X,
    Q_Y,
    Q_Z,
    Q_W
};

typedef qreal* xmmfloat;

typedef Marble::xmmfloat* matrix;


class Quaternion
{
%TypeHeaderCode
#include <Quaternion.h>
%End

public:
                            Quaternion ();
                            Quaternion (qreal w, qreal x, qreal y, qreal z);
    Marble::Quaternion      operator * (const Marble::Quaternion& q) const;
    bool                    operator == (const Marble::Quaternion& q) const;
    Marble::Quaternion&     operator *= (const Marble::Quaternion& q);

    void                    getSpherical (qreal& lon /out/, qreal& lat /out/) const;

    void                    normalize ();
    Marble::Quaternion      inverse () const;
    qreal                   pitch () const;
    qreal                   yaw () const;
    qreal                   roll () const;
    void                    rotateAroundAxis (const Marble::Quaternion& q /in/);
    void                    slerp (const Marble::Quaternion& q1, const Marble::Quaternion& q2, qreal t);
    void                    nlerp (const Marble::Quaternion& q1, const Marble::Quaternion& q2, qreal t);
//FIXME    void                    toMatrix (Marble::matrix& m) const;
//FIXME    void                    rotateAroundAxis (const Marble::matrix& m);
//FIXME    Marble::xmmfloat v;
};
// Quaternion

};
// Marble


