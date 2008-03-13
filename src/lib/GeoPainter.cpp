//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2006-2008 Torsten Rahn <tackat@kde.org>"


#include "GeoPainter.h"

#include <QtCore/QDebug>


// #define MARBLE_DEBUG

GeoPainter::GeoPainter( MarbleMap * map, bool clip)
    : ClipPainter()
{
}

void GeoPainter::drawPoint (  const GeoDataPoint & position )
{
}

void GeoPainter::drawPoints (  const GeoDataPoint * points, int pointCount )
{
}

void GeoPainter::drawText ( const GeoDataPoint & position, const QString & text )
{
}

void GeoPainter::drawText ( const GeoDataPoint & position, const QString & text )
{
}

