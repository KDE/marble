//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009      Eckhart Wörner <ewoerner@kde.org>
//

#ifndef GEOCUTE_VELOCITY_H
#define GEOCUTE_VELOCITY_H

#include <QFlags>



namespace GeoCute
{
    
enum VelocityFieldFlag {
    VelocityFieldsNone = 0,
    VelocityFieldsSpeed = 1 << 0,
    VelocityFieldsDirection = 1 << 1,
    VelocityFieldsClimb = 1 << 2
};

Q_DECLARE_FLAGS(VelocityFields, VelocityFieldFlag)
Q_DECLARE_OPERATORS_FOR_FLAGS(VelocityFields)

struct Velocity
{
    VelocityFields fields;
    qreal speed;
    qreal direction;
    qreal climb;
};
    
}



#endif
