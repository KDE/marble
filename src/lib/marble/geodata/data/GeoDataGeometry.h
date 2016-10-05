//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2008      Torsten Rahn <rahn@kde.org>
// Copyright 2008-2009      Patrick Spendrin <ps_ml@gmx.de>
// Copyright 2008      Inge Wallin <inge@lysator.liu.se>
//


#ifndef MARBLE_GEODATAGEOMETRY_H
#define MARBLE_GEODATAGEOMETRY_H


#include "GeoDataObject.h"
#include "MarbleGlobal.h"

#include "geodata_export.h"

namespace Marble
{

class GeoDataGeometryPrivate;

class GeoDataLatLonAltBox;
class GeoDataLineString; // LinearRing is the same!
class GeoDataMultiGeometry;

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
    GeoDataGeometry();
    GeoDataGeometry( const GeoDataGeometry& other );
    GeoDataGeometry& operator=( const GeoDataGeometry& other );
    
    virtual ~GeoDataGeometry();

    /// Provides type information for downcasting a GeoData
    virtual const char* nodeType() const;
    virtual EnumGeometryId geometryId() const;

    bool extrude() const;
    void setExtrude( bool extrude );

    AltitudeMode altitudeMode() const;
    void setAltitudeMode( const AltitudeMode altitudeMode );

    virtual const GeoDataLatLonAltBox& latLonAltBox() const;

    /// Serialize the contents of the feature to @p stream.
    virtual void pack( QDataStream& stream ) const;
    /// Unserialize the contents of the feature from @p stream.
    virtual void unpack( QDataStream& stream );

    void detach();

 protected:
    explicit GeoDataGeometry( GeoDataGeometryPrivate* priv );

    bool equals(const GeoDataGeometry &other) const;

    using GeoDataObject::equals;

 protected:
    GeoDataGeometryPrivate *d_ptr;
};

}

Q_DECLARE_METATYPE( Marble::GeoDataGeometry* )

#endif
