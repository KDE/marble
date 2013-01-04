//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2006-2007 Torsten Rahn <tackat@kde.org>
// Copyright 2007      Inge Wallin  <ingwa@kde.org>
// Copyright 2008-2009      Patrick Spendrin <ps_ml@gmx.de>
//


#ifndef MARBLE_GEODATAPLACEMARK_H
#define MARBLE_GEODATAPLACEMARK_H


#include <QtCore/QChar>
#include <QtCore/QDateTime>
#include <QtXml/QXmlStreamWriter>

#include "GeoDataPoint.h"
#include "GeoDataLineString.h"
#include "GeoDataLinearRing.h"
#include "GeoDataLookAt.h"
#include "GeoDataPolygon.h"
#include "GeoDataMultiGeometry.h"
#include "GeoDataFeature.h"

#include "geodata_export.h"

namespace Marble
{

class GeoDataPlacemarkPrivate;

/**
 * @short a class representing a point of interest on the map
 *
 * This class represents a point of interest, e.g. a city or a
 * mountain.  It is filled with data by the KML or GPX loader and the
 * PlacemarkModel makes use of it.
 *
 * A Placemark can have an associated geometry which will be rendered to the map
 * along with the placemark's point icon. If you would like to render more than
 * one geometry for any one placemark than use @see setGeometry() to set add a
 * @see MultiGeometry.
 *
 * This is more or less only a GeoDataFeature with a geographic
 * position and a country code attached to it.  The country code is
 * not provided in a KML file.
 */

class GEODATA_EXPORT GeoDataPlacemark: public GeoDataFeature
{
 public:
    /**
     * Create a new placemark.
     */
    GeoDataPlacemark();

    /**
     * Create a new placemark from existing placemark @p placemark
     */
    GeoDataPlacemark( const GeoDataPlacemark& placemark );

    /**
     * Create a new placemark with the given @p name.
     */
    explicit GeoDataPlacemark( const QString &name );

    /**
    * Delete the placemark
    */
    ~GeoDataPlacemark();

    /**
    * comparison operator is implemented slightly different than one would expect.
    * Only Placemarks that are copies of each other are assumed to be equal.
    */
    bool operator==( const GeoDataPlacemark& other ) const;

    /**
     * Return the coordinates of the placemark at time @p dateTime as a GeoDataCoordinates
     *
     * The @p dateTime parameter should be used if the placemark geometry() is a
     * GeoDataTrack and thus contains several coordinates associated with a date and time.
     *
     * The @p iconAtCoordinates boolean is set to true if an icon should be drawn to
     * represent the placemark at these coordinates as described in
     * https://code.google.com/apis/kml/documentation/kmlreference.html#placemark,
     * it is set to false otherwise.
     *
     * @see GeoDataTrack::GeoDataTrack
     */
    GeoDataCoordinates coordinate( const QDateTime &dateTime = QDateTime(), bool *iconAtCoordinates = 0 ) const;

    /**
     * The geometry of the GeoDataPlacemark is to be rendered to the marble map
     * along with the icon at the coordinate associated with this Placemark.
     * @return a pointer to the current Geometry object
     */
    GeoDataGeometry* geometry() const;
    
    /**
     * Return the coordinates of the placemark as @p longitude,
     * @p latitude and @p altitude.
     */
    void coordinate( qreal &longitude, qreal &latitude, qreal &altitude ) const;

    /**
     * Set the coordinate of the placemark in @p longitude and
     * @p latitude.
     */
    void setCoordinate( qreal longitude, qreal latitude, qreal altitude = 0,
                        GeoDataCoordinates::Unit _unit = GeoDataCoordinates::Radian );

    /**
    * Set the coordinate of the placemark with an @p GeoDataPoint.
    */
    void setCoordinate( const GeoDataCoordinates &coordinate );

    MARBLE_DEPRECATED( void setCoordinate( const GeoDataPoint &point ) );

    /**
     * Sets the current Geometry of this Placemark. @see geometry() and the class 
     * overview for description of the geometry concept. The geometry can be set 
     * to any @see GeoDataGeometry like @see GeoDataPoint,@see GeoDataLineString,
     * @see GeoDataLinearRing and @see GeoDataMultiGeometry
     */
    void setGeometry( GeoDataGeometry *entry );

    /**
     * Return the area size of the feature in square km.
     *
     * FIXME: Once we make Marble more area-aware we need to 
     * move this into the GeoDataArea class which will get
     * inherited from GeoDataPlacemark (or GeoDataFeature).
     */
    qreal area() const;

    /**
     * Set the area size of the feature in square km.
     */
    void setArea( qreal area );

    /**
     * Return the population of the placemark.
     */
    qint64 population() const;
    /**
     * Sets the @p population of the placemark.
     * @param  population  the new population value
     */
    void setPopulation( qint64 population );

    /**
     * Return the state of the placemark.
     */
    const QString state() const;

    /**
     * Set the state @p state of the placemark.
     */
    void setState( const QString &state );

    /**
     * Return the country code of the placemark.
     */
    const QString countryCode() const;

    /**
     * Set the country @p code of the placemark.
     */
    void setCountryCode( const QString &code );

    /**
     * Serialize the Placemark to a data stream. This is a binary serialisation
     * and is deserialised using @see unpack()
     * @param stream the QDataStream to serialise object to.
     */
    virtual void pack( QDataStream& stream ) const;

    /**
     * Serialise this Placemark to a XML stream writer @see QXmlStreamWriter in
     * the Qt documentation for more info. This will output the XML
     * representation of this Placemark. The default XML format is KML, to have
     * other formats supported you need to create a subclass and override this
     * method.
     * @param stream the XML Stream Reader to output to.
     */
    virtual QXmlStreamWriter& pack( QXmlStreamWriter& stream ) const;

    virtual QXmlStreamWriter& operator <<( QXmlStreamWriter& stream ) const;

    /**
     * Deserialize the Placemark from a data stream. This has the opposite effect
     * from @see pack()
     * @param stream the QDataStream to deserialise from.
     */
    virtual void unpack( QDataStream& stream );

    /**
     * Returns GeoDataLookAt object if lookAt is setup earlier
     * otherwise It will convert GeoDataCoordinates of Placemark
     * to GeoDataLookAt with range equals to altitude of
     * GeoDataCoordinate
     */
    GeoDataLookAt *lookAt() const;

    /**
     * Set lookAt @p GeoDataLookAt
     */
    void setLookAt( GeoDataLookAt *lookAt );

 private:
    GeoDataPlacemarkPrivate *p() const;
};

}

#endif
