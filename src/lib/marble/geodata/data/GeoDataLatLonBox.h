// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2007 Andrew Manson <g.real.ate@gmail.com>
// SPDX-FileCopyrightText: 2008-2009 Torsten Rahn <rahn@kde.org>
//


#ifndef MARBLE_GEODATALATLONBOX_H
#define MARBLE_GEODATALATLONBOX_H


#include "MarbleGlobal.h"

#include "GeoDataObject.h"
#include "GeoDataCoordinates.h"

#include "geodata_export.h"

namespace Marble
{

class GeoDataLatLonBoxPrivate;

class GeoDataLineString;

/**
 * @short A class that defines a 2D bounding box for geographic data.
 *
 * GeoDataLatLonBox is a 2D bounding box that describes a geographic area
 * in terms of latitude and longitude.
 *
 * The bounding box gets described by assigning the northern, southern, 
 * eastern and western boundary.
 * So usually the value of the eastern boundary is bigger than the
 * value of the western boundary.
 *
 * This is also true if the GeoDataLatLonBox covers the whole longitude
 * range from 180 deg West to 180 deg East. Notably in this case
 * the bounding box crosses the date line.
 *
 * If the GeoDataLatLonBox does not cover the whole longitude range but still
 * crosses the date line then the eastern boundary has got a smaller value than
 * the western one.
 */

class GEODATA_EXPORT GeoDataLatLonBox : public GeoDataObject
{
    friend bool GEODATA_EXPORT operator==( GeoDataLatLonBox const& lhs, GeoDataLatLonBox const& rhs );
    friend bool GEODATA_EXPORT operator!=( GeoDataLatLonBox const& lhs, GeoDataLatLonBox const& rhs );

 public:
    GeoDataLatLonBox();
    GeoDataLatLonBox( qreal north, qreal south, qreal east, qreal west, GeoDataCoordinates::Unit unit = GeoDataCoordinates::Radian );
    GeoDataLatLonBox( const GeoDataLatLonBox & );
    ~GeoDataLatLonBox() override;

    GeoDataLatLonBox& operator=( const GeoDataLatLonBox& other );

    /// Provides type information for downcasting a GeoData
    const char* nodeType() const override;

    /**
     * @brief Get the northern boundary of the bounding box.
     * @return the latitude of the northern boundary.
     */
    qreal north( GeoDataCoordinates::Unit unit = GeoDataCoordinates::Radian ) const;
    void setNorth( const qreal north, GeoDataCoordinates::Unit unit = GeoDataCoordinates::Radian );

    /**
     * @brief Get the southern boundary of the bounding box.
     * @return the latitude of the southern boundary.
     */
    qreal south( GeoDataCoordinates::Unit unit = GeoDataCoordinates::Radian ) const;
    void setSouth( const qreal south, GeoDataCoordinates::Unit unit = GeoDataCoordinates::Radian );

    /**
     * @brief Get the eastern boundary of the bounding box.
     * @return the longitude of the eastern boundary.
     */
    qreal east( GeoDataCoordinates::Unit unit = GeoDataCoordinates::Radian ) const;
    void setEast( const qreal east, GeoDataCoordinates::Unit unit = GeoDataCoordinates::Radian );

    /**
     * @brief Get the western boundary of the bounding box.
     * @return the longitude of the western boundary.
     */
    qreal west( GeoDataCoordinates::Unit unit = GeoDataCoordinates::Radian ) const;
    void setWest( const qreal west, GeoDataCoordinates::Unit unit = GeoDataCoordinates::Radian );

    /**
     * @brief Get the rotation of the bounding box.
     * @return the rotation of the bounding box.
     */
    qreal rotation( GeoDataCoordinates::Unit unit = GeoDataCoordinates::Radian ) const;
    void setRotation( const qreal rotation, GeoDataCoordinates::Unit unit = GeoDataCoordinates::Radian );

    void boundaries( qreal &north, qreal &south, qreal &east, qreal &west, GeoDataCoordinates::Unit unit = GeoDataCoordinates::Radian ) const;
    void setBoundaries( qreal north, qreal south, qreal east, qreal west, GeoDataCoordinates::Unit unit = GeoDataCoordinates::Radian );

    /**
     * @brief Changes the differences between the boundaries and the center by the given factor, keeping the center unchanged
     * @param verticalFactor Vertical scale factor (affects north and south boundaries)
     * @param horizontalFactor Horizontal scale factor (affects west and east boundaries)
     */
    void scale(qreal verticalFactor, qreal horizontalFactor) const;
    GeoDataLatLonBox scaled(qreal verticalFactor, qreal horizontalFactor) const;

    /**
     * @brief Get the width of the longitude interval
     * @return the angle covered by the longitude range.
     */
    qreal width( GeoDataCoordinates::Unit unit = GeoDataCoordinates::Radian ) const;

    /**
     * @brief Get the width of the longitude interval. East and west parameters are in radians.
     * @return the angle covered by the longitude range in given unit.
     */
    static qreal width( qreal east, qreal west, GeoDataCoordinates::Unit unit = GeoDataCoordinates::Radian );

    /**
     * @brief Get the height of the latitude interval
     * @return the angle covered by the latitude range.
     */
    qreal height( GeoDataCoordinates::Unit unit = GeoDataCoordinates::Radian ) const;

    /**
     * @brief Get the height of the latitude interval. North and south parameters are in radians.
     * @return the angle covered by the latitude range in given unit.
     */
    static qreal height( qreal north, qreal south, GeoDataCoordinates::Unit unit = GeoDataCoordinates::Radian );

    /**
     * @brief Detect whether the bounding box crosses the IDL.
     * @return @c true  the bounding box crosses the +/-180 deg longitude.
     *         @c false the bounding box doesn't cross the +/-180 deg longitude.
     */
    bool     crossesDateLine() const;

    /**
     * @brief Detect whether the bounding box crosses the IDL.
     * @param east radians east.
     * @param west radians west.
     * @return @c true  the bounding box crosses the +/-180 deg longitude.
     *         @c false the bounding box doesn't cross the +/-180 deg longitude.
     */
    static bool crossesDateLine(qreal east, qreal west);

    /**
     * @brief returns the center of this box
     * @return a coordinate, face-center of the box
     */
    virtual GeoDataCoordinates center() const;

    /**
     * @brief Detect whether the bounding box contains one of the poles.
     * @return @c true  the bounding box contains one of the poles.
     *         @c false the bounding box doesn't contain one of the poles.
     */
    bool containsPole( Pole pole = AnyPole ) const;

    virtual bool contains( const GeoDataCoordinates & ) const;
    bool     contains( const GeoDataLatLonBox & ) const;

    /**
     * @brief Detect whether the bounding box contains a point of given lon and lat.
     * @param lon longitude in radians.
     * @param lat latitude in radians.
     * @return true if the box contains given point, false otherwise
     */
    bool contains(qreal lon, qreal lat) const; //Optimized version for overlay painting

    virtual bool intersects( const GeoDataLatLonBox & ) const;

    /**
     * @brief Returns the bounding LatLonBox of this box with the given one.
     */
    GeoDataLatLonBox united( const GeoDataLatLonBox& other) const;

    /**
     * @return Returns the smallest bounding box that contains this LatLonBox rotated with its given angle.
     */
    GeoDataLatLonBox toCircumscribedRectangle() const;

    /**
     * @brief Create the smallest bounding box from a line string.
     * @return the smallest bounding box that contains the linestring.
     */
    static GeoDataLatLonBox fromLineString( const GeoDataLineString& lineString );

    /**
     * @brief Indicates whether the bounding box only contains a single 2D point ("singularity").
     * @return Return value is true if the height and the width of the bounding box equal zero.
     */
    virtual bool isNull() const;

    /**
     * @brief Indicates whether the bounding box is not initialised (and contains nothing).
     * @return Return value is true if bounding box is not initialised.
     */
    virtual bool isEmpty() const;

    /**
     * @brief Indicates whether two bounding boxes are roughly equal.
     *        The factor specifies the margin threshold relative to the left handside
     *        bounding box within which both bounding boxes are considered equal.
     * @return Return value is true if both bounding box are approximately equal.
     */
    static bool fuzzyCompare(const GeoDataLatLonBox& lhs,
                             const GeoDataLatLonBox& rhs,
                             const qreal factor = 0.01);

    /**
     * @brief Resets the bounding box to its uninitialised state (and thus contains nothing).
     */
    virtual void clear();

    GeoDataLatLonBox operator|( const GeoDataLatLonBox& other ) const;

    /**
     * @brief Unites this bounding box with the given one.
     * @return Returns a reference to self.
     */
    GeoDataLatLonBox& operator |=( const GeoDataLatLonBox& other) ;

    /// Serialize the contents of the feature to @p stream.
    void pack( QDataStream& stream ) const override;
    /// Unserialize the contents of the feature from @p stream.
    void unpack( QDataStream& stream ) override;

 private:
    GeoDataLatLonBoxPrivate  * const d;
    static const GeoDataLatLonBox empty;
};

}

Q_DECLARE_METATYPE( Marble::GeoDataLatLonBox )

#endif
