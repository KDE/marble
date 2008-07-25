//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007      Andrew Manson  <g.real.ate@gmail.com>
// Copyright 2008      Torsten Rahn   <rahn@kde.org>
//


#ifndef GEODATALATLONALTBOX_H
#define GEODATALATLONALTBOX_H


#include "global.h"

#include "GeoDataObject.h"
#include "GeoDataPoint.h"

#include "geodata_export.h"


using namespace Marble;


class GeoDataLatLonBoxPrivate;

/**
 * @short A class that defines a 2D bounding box for geographic data.
 *
 * GeoDataLatLonBox is a 2D bounding box that describes a geographic area
 * in terms of latidude and longitude.
 *
 * Note that the bounding box itself doesn't get calculated from the 
 * original list of points in this class. Instead it should get calculated 
 * in the GeoDataLineString class (or similar) for performance reasons.
 */

class GEODATA_EXPORT GeoDataLatLonBox : public GeoDataObject
{
    friend bool operator==( GeoDataLatLonBox const& lhs, GeoDataLatLonBox const& rhs );

 public:
    GeoDataLatLonBox();
    GeoDataLatLonBox( double north, double south, double east, double west, GeoDataPoint::Unit unit = GeoDataPoint::Radian );
    GeoDataLatLonBox( const GeoDataLatLonBox & );
    virtual ~GeoDataLatLonBox();

    virtual bool isFolder() const { return false; }

    GeoDataLatLonBox& operator=( const GeoDataLatLonBox& other );

    /**
     * @brief Get the northern boundary of the bounding box.
     * @return the latitude of the northern boundary.
     */
    double north( GeoDataPoint::Unit unit = GeoDataPoint::Radian ) const;
    void setNorth( const double north, GeoDataPoint::Unit unit = GeoDataPoint::Radian );

    /**
     * @brief Get the southern boundary of the bounding box.
     * @return the latitude of the southern boundary.
     */
    double south( GeoDataPoint::Unit unit = GeoDataPoint::Radian ) const;
    void setSouth( const double south, GeoDataPoint::Unit unit = GeoDataPoint::Radian );

    /**
     * @brief Get the eastern boundary of the bounding box.
     * @return the longitude of the eastern boundary.
     */
    double east( GeoDataPoint::Unit unit = GeoDataPoint::Radian ) const;
    void setEast( const double east, GeoDataPoint::Unit unit = GeoDataPoint::Radian );

    /**
     * @brief Get the western boundary of the bounding box.
     * @return the longitude of the western boundary.
     */
    double west( GeoDataPoint::Unit unit = GeoDataPoint::Radian ) const;
    void setWest( const double west, GeoDataPoint::Unit unit = GeoDataPoint::Radian );

    /**
     * @brief Get the rotation of the bounding box.
     * @return the rotation of the bounding box.
     */
    double rotation( GeoDataPoint::Unit unit = GeoDataPoint::Radian ) const;
    void setRotation( const double rotation, GeoDataPoint::Unit unit = GeoDataPoint::Radian );

    void    boundaries( double &west, double &east, double &north, double &south, GeoDataPoint::Unit unit = GeoDataPoint::Radian );
    void    setBoundaries( double west, double east, double north, double south, GeoDataPoint::Unit unit = GeoDataPoint::Radian );

    /**
     * @brief Detect whether the bounding box crosses the IDL.
     * @return @c true  the bounding box crosses the +/+180 deg longitude.
     *         @c false the bounding box doesn't cross the +/+180 deg longitude.
     */
    bool crossesDateLine() const;

    bool     virtual contains( const GeoDataPoint & );
    bool     virtual intersects( const GeoDataLatLonBox & );

    /**
     * @brief Dumps the boundaries of the bounding box for debugging purpose.
     */
    QString  virtual text( GeoDataPoint::Unit unit = GeoDataPoint::Radian ) const;

    /// Serialize the contents of the feature to @p stream.
    virtual void pack( QDataStream& stream ) const;
    /// Unserialize the contents of the feature from @p stream.
    virtual void unpack( QDataStream& stream );

 private:
    GeoDataLatLonBoxPrivate  * const d;
};

class GeoDataLatLonAltBoxPrivate;

/**
 * @short A class that defines a 3D bounding box for geographic data.
 *
 * GeoDataLatLonAltBox is a 3D bounding box that describes a geographic area
 * in terms of latidude, longitude and altitude.
 *
 * Note that the bounding box itself doesn't get calculated from the 
 * original list of points in this class. Instead it should get calculated 
 * in the GeoDataLineString class (or similar) for performance reasons.
 */

class GEODATA_EXPORT GeoDataLatLonAltBox : public GeoDataLatLonBox
{
    friend bool GEODATA_EXPORT operator==( GeoDataLatLonAltBox const& lhs, GeoDataLatLonAltBox const& rhs );

 public:
    GeoDataLatLonAltBox();
    GeoDataLatLonAltBox( const GeoDataLatLonAltBox & );
    virtual ~GeoDataLatLonAltBox();

    virtual bool isFolder() const { return false; }

    GeoDataLatLonAltBox& operator=( const GeoDataLatLonAltBox& other );

    /**
     * @brief Get the lower altitude boundary of the bounding box.
     * @return the height of the lower altitude boundary.
     */
    double minAltitude() const;
    void setMinAltitude( const double minAltitude );

    /**
     * @brief Get the upper altitude boundary of the bounding box.
     * @return the height of the upper altitude boundary.
     */
    double maxAltitude() const;
    void setMaxAltitude( const double maxAltitude );

    /**
     * @brief Get the reference system for the altitude.ar
     * @return the point of reference which marks the origin 
     * for measuring the altitude.
     */
    AltitudeMode altitudeMode() const;
    void setAltitudeMode( const AltitudeMode altitudeMode );

    bool     virtual contains( const GeoDataPoint & );
    bool     virtual intersects( const GeoDataLatLonAltBox & );
    using GeoDataLatLonBox::intersects;

    /**
     * @brief Dumps the boundaries of the bounding box for debugging purpose.
     */
    QString  virtual text( GeoDataPoint::Unit unit = GeoDataPoint::Radian ) const;

    /// Serialize the contents of the feature to @p stream.
    virtual void pack( QDataStream& stream ) const;
    /// Unserialize the contents of the feature from @p stream.
    virtual void unpack( QDataStream& stream );

 private:
    GeoDataLatLonAltBoxPrivate  * const d;
};

bool GEODATA_EXPORT operator==( GeoDataLatLonAltBox const& lhs, GeoDataLatLonAltBox const& rhs );

#endif // GEODATALATLONALTBOX
