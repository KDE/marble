/*
    SPDX-FileCopyrightText: 2008 Torsten Rahn <tackat@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "MathHelper.h"

#ifdef Q_CC_MSVC
#include <float.h>

qreal msvc_asinh(qreal x)
{
    if ( _isnan ( x ) ) {
        errno = EDOM;
        return x;
    }

    return ( log( x + sqrt ( x * x + 1.0 ) ) );
}

qreal msvc_atanh(qreal x)
{
    return ( 0.5 * log( ( 1.0 + x ) / ( 1.0 - x ) ) );
}

#endif  // Q_CC_MSVC

