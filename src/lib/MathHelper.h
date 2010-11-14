/**
 * This file is part of the Marble Virtual Globe.
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

#ifndef MATHHELPER_H
#define MATHHELPER_H

#include <QtCore/qglobal.h>

#ifdef Q_CC_MSVC
#include <math.h>

qreal msvc_asinh(qreal x);
#define asinh msvc_asinh

qreal msvc_atanh(qreal x);
#define atanh msvc_atanh
#endif

#endif  // MATHHELPER_H
