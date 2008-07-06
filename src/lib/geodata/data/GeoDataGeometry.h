//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2008      Torsten Rahn <rahn@kde.org>
// Copyright 2008      Patrick Spendrin <ps_ml@gmx.de>"
//


#ifndef GEODATAGEOMETRY_H
#define GEODATAGEOMETRY_H


#include "GeoDataObject.h"
#include "global.h"

#include "geodata_export.h"

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

using namespace Marble;

class GeoDataGeometryPrivate;

class GEODATA_EXPORT GeoDataGeometry : public GeoDataObject
{
 public:
    virtual bool isFolder() const { return false; }

    bool extrude() const;
    void setExtrude( bool extrude );

    bool tessellate() const;
    void setTessellate( bool tessellate );

    AltitudeMode altitudeMode() const;
    void setAltitudeMode( const AltitudeMode altitudeMode );

    GeoDataGeometry();
    GeoDataGeometry( const GeoDataGeometry & other );
    GeoDataGeometry& operator=( const GeoDataGeometry& other );
    
    virtual ~GeoDataGeometry();

 protected:
    GeoDataGeometryPrivate* const d_geom;
};

#endif // GEODATAGEOMETRY_H
