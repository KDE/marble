//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007      Torsten Rahn <tackat@kde.org>"
// Copyright 2007      Inge Wallin  <ingwa@kde.org>"
//


#ifndef GLOBAL_H
#define GLOBAL_H


#include <math.h>


#include <QtCore/QObject>

// This enum is used to choose projection shown in the view.

enum Projection { 
    Spherical,
    Equirectangular
    // Mercator
};

// Define FLAT_PROJ here to get flat projection.
//
//#define FLAT_PROJ


const double DEG2RAD = M_PI / 180.0;
const double RAD2DEG = 180.0 / M_PI;

const QString NOT_AVAILABLE = QObject::tr("not available");

// Usage deprecated:
const double RAD2INT = 21600.0 / M_PI;

const double EARTH_RADIUS = 6378.0;

#endif
