/**
 * This file is part of the Marble Desktop Globe.
 *
 * Copyright 2008 Torsten Rahn <tackat@kde.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
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

