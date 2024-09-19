/*
    SPDX-FileCopyrightText: 2008 Torsten Rahn <tackat@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef MATHHELPER_H
#define MATHHELPER_H

#include <qglobal.h>

#ifdef Q_CC_MSVC
#include <math.h>

qreal msvc_asinh(qreal x);
#define asinh msvc_asinh

qreal msvc_atanh(qreal x);
#define atanh msvc_atanh
#endif

#endif // MATHHELPER_H
