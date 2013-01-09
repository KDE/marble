//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007      Andrew Manson  <g.real.ate@gmail.com>
// Copyright 2008-2009 Torsten Rahn   <rahn@kde.org>
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
    friend bool operator==( GeoDataLatLonBox const& lhs, GeoDataLatLonBox const& rhs );

 public:
    GeoDataLatLonBox();
    GeoDataLatLonBox( qreal north, qreal south, qreal east, qreal west, GeoDataCoordinates::Unit unit = GeoDataCoordinates::Radian );
    GeoDataLatLonBox( const GeoDataLatLonBox & );
    virtual ~GeoDataLatLonBox();

    GeoDataLatLonBox& operator=( const GeoDataLatLonBox& other );

    /// Provides type information for downcasting a GeoData
    virtual const char* nodeType() const;

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

    void    boundaries( qreal &north, qreal &south, qreal &east, qreal &west, GeoDataCoordinates::Unit unit = GeoDataCoordinates::Radian ) const;
    void    setBoundaries( qreal north, qreal south, qreal east, qreal west, GeoDataCoordinates::Unit unit = GeoDataCoordinates::Radian );

    /**
     * @brief Get the width of the longitude interval
     * @return the angle covered by the longitude range.
     */
    qreal width( GeoDataCoordinates::Unit unit = GeoDataCoordinates::Radian ) const;

    /**
     * @brief Get the height of the latitude interval
     * @return the angle covered by the latitude range.
     */
    qreal height( GeoDataCoordinates::Unit unit = GeoDataCoordinates::Radian ) const;

    /**
     * @brief Detect whether the bounding box crosses the IDL.
     * @return @c true  the bounding box crosses the +/-180 deg longitude.
     *         @c false the bounding box doesn't cross the +/-180 deg longitude.
     */
    bool     crossesDateLine() const;

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

    virtual bool intersects( const GeoDataLatLonBox & ) const;

    /**
     * @brief Returns the bounding LatLonBox of this box with the given one.
     */
    GeoDataLatLonBox united( const GeoDataLatLonBox& other) const;

    /**
     * @brief Create the smallest bounding box from a line string.
     * @return the smallest bounding box that contains the linestring.
     */
    static GeoDataLatLonBox fromLineString( const GeoDataLineString& lineString );

    /**
     * @brief Creates a text string of the bounding box
     */
    virtual QString toString( GeoDataCoordinates::Unit unit = GeoDataCoordinates::Radian ) const;

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
    virtual void pack( QDataStream& stream ) const;
    /// Unserialize the contents of the feature from @p stream.
    virtual void unpack( QDataStream& stream );

 private:
    GeoDataLatLonBoxPrivate  * const d;
    static const GeoDataLatLonBox empty;
};

}

Q_DECLARE_METATYPE( Marble::GeoDataLatLonBox )

#endif
