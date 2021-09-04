// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2006-2007 Torsten Rahn <tackat@kde.org>
// SPDX-FileCopyrightText: 2007 Inge Wallin <ingwa@kde.org>
// SPDX-FileCopyrightText: 2008 Patrick Spendrin <ps_ml@gmx.de>
//


#ifndef MARBLE_GEODATAPOINT_H
#define MARBLE_GEODATAPOINT_H

#include <QMetaType>
#include <QVector>

#include <cmath>

#include "geodata_export.h"
#include "GeoDataGeometry.h"
#include "GeoDataCoordinates.h"

namespace Marble
{

class GeoDataPointPrivate;

/** 
 * @short A Geometry object representing a 3d point
 *
 * GeoDataPoint is the GeoDataGeometry class representing a single three
 * dimensional point. It reflects the Point tag of KML spec and can be contained
 * in objects holding GeoDataGeometry objects. 
 * Nevertheless GeoDataPoint shouldn't be used if you just want to store 
 * 3d coordinates of a point that doesn't need to be inherited from GeoDataGeometry
 * In that case use GeoDataCoordinates instead which has nearly the same features 
 * and is much more light weight. 
 * Please consider this especially if you expect to have a high
 * amount of points e.g. for line strings, linear rings and polygons.
 * @see GeoDataCoordinates
 * @see GeoDataGeometry
*/

class GEODATA_EXPORT GeoDataPoint : public GeoDataGeometry
{
 public:
    using Notation = GeoDataCoordinates::Notation;
    using Unit = GeoDataCoordinates::Unit;

    GeoDataPoint( const GeoDataPoint& other );
    explicit GeoDataPoint( const GeoDataCoordinates& other );
    GeoDataPoint();

    /**
     * @brief create a geopoint from longitude and latitude
     * @param lon longitude
     * @param lat latitude
     * @param alt altitude (default: 0)
     * @param _unit units that lon and lat get measured in
     * (default for Radian: north pole at pi/2, southpole at -pi/2)
     */
    GeoDataPoint( qreal lon, qreal lat, qreal alt = 0,
                  GeoDataPoint::Unit _unit = GeoDataCoordinates::Radian );

    ~GeoDataPoint() override;

    EnumGeometryId geometryId() const override;

    GeoDataGeometry *copy() const override;

    bool operator==( const GeoDataPoint &other ) const;
    bool operator!=( const GeoDataPoint &other ) const;

    void setCoordinates( const GeoDataCoordinates &coordinates );

    const GeoDataCoordinates& coordinates() const;

    /// Provides type information for downcasting a GeoData
    const char* nodeType() const override;

    // Type definitions
    using Vector = QVector<GeoDataPoint>;


    // Serialize the Placemark to @p stream
    void pack( QDataStream& stream ) const override;
    // Unserialize the Placemark from @p stream
    void unpack( QDataStream& stream ) override;

    virtual void detach();

 private:
    Q_DECLARE_PRIVATE(GeoDataPoint)
};

}

Q_DECLARE_METATYPE( Marble::GeoDataPoint )

#endif
