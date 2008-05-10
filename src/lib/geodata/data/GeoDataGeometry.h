//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2008      Torsten Rahn <rahn@kde.org>
//


#ifndef GEODATAGEOMETRY_H
#define GEODATAGEOMETRY_H


#include "GeoDataObject.h"


/**
 * @short A base class for all geodata features
 *
 * GeoDataGeometry is the base class for most geodata classes that
 * deal with geometric data (points, linestrings, polygons, etc.)
 *
 * @see GeoDataLineString
 * @see GeoDataLinearRing
 * @see GeoDataPolygon
 */

class GeoDataGeometry : public GeoDataObject
{
 public:
    virtual bool isFolder() const { return false; }

    virtual ~GeoDataGeometry();

 protected:
    GeoDataGeometry();
};

#endif // GEODATAGEOMETRY_H
