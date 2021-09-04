// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2008 Torsten Rahn <rahn@kde.org>
// SPDX-FileCopyrightText: 2008-2009 Patrick Spendrin <ps_ml@gmx.de>
// SPDX-FileCopyrightText: 2008 Inge Wallin <inge@lysator.liu.se>
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
    ~GeoDataGeometry() override;

    virtual EnumGeometryId geometryId() const = 0;

    virtual GeoDataGeometry *copy() const = 0;

    bool operator==(const GeoDataGeometry &other) const;
    inline bool operator!=(const GeoDataGeometry &other) const { return !(*this == other); }

    bool extrude() const;
    void setExtrude( bool extrude );

    AltitudeMode altitudeMode() const;
    void setAltitudeMode( const AltitudeMode altitudeMode );

    virtual const GeoDataLatLonAltBox& latLonAltBox() const;

    /// Serialize the contents of the feature to @p stream.
    void pack( QDataStream& stream ) const override;
    /// Unserialize the contents of the feature from @p stream.
    void unpack( QDataStream& stream ) override;

    void detach();

 protected:
    explicit GeoDataGeometry( GeoDataGeometryPrivate* priv );
    explicit GeoDataGeometry(const GeoDataGeometry &other);
    GeoDataGeometry& operator=(const GeoDataGeometry &other);

    bool equals(const GeoDataGeometry &other) const;

    using GeoDataObject::equals;

 protected:
    GeoDataGeometryPrivate *d_ptr;
};

}

Q_DECLARE_METATYPE( Marble::GeoDataGeometry* )

#endif
