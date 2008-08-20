//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2008      Torsten Rahn <rahn@kde.org>
// Copyright 2008      Patrick Spendrin <ps_ml@gmx.de>
// Copyright 2008      Inge Wallin <inge@lysator.liu.se>
//


#ifndef GEODATAGEOMETRY_H
#define GEODATAGEOMETRY_H


#include "GeoDataObject.h"
#include "global.h"

#include "geodata_export.h"

namespace Marble
{

class GeoDataGeometryPrivate;

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

class GEODATA_EXPORT GeoDataGeometry : public GeoDataObject
{
 public:
    virtual bool isFolder() const { return false; }
    virtual EnumGeometryId geometryId() const { return InvalidGeometryId; };

    bool extrude() const;
    void setExtrude( bool extrude );

    bool tessellate() const;
    void setTessellate( bool tessellate );

    AltitudeMode altitudeMode() const;
    void setAltitudeMode( const AltitudeMode altitudeMode );

    explicit GeoDataGeometry( GeoDataObject *parent = 0 );
    GeoDataGeometry( const GeoDataGeometry & other );
    GeoDataGeometry& operator=( const GeoDataGeometry& other );
    
    virtual ~GeoDataGeometry();

    /// Serialize the contents of the feature to @p stream.
    virtual void pack( QDataStream& stream ) const;
    /// Unserialize the contents of the feature from @p stream.
    virtual void unpack( QDataStream& stream );

 protected:
    GeoDataGeometryPrivate* const d;
};

}

#endif // GEODATAGEOMETRY_H
