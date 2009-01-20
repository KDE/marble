//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2006-2007 Torsten Rahn <tackat@kde.org>
// Copyright 2007      Inge Wallin  <ingwa@kde.org>
// Copyright 2008      Patrick Spendrin <ps_ml@gmx.de>
//


#ifndef GEODATAPLACEMARK_H
#define GEODATAPLACEMARK_H


#include <QtCore/QChar>

#include "GeoDataPoint.h"
#include "GeoDataLineString.h"
#include "GeoDataLinearRing.h"
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
    explicit GeoDataPlacemark( GeoDataObject* parent = 0 );

    /**
     * Create a new placemark from existing placemark @p placemark
     */
    GeoDataPlacemark( const GeoDataPlacemark& placemark );

    /**
     * Create a new placemark with the given @p name.
     */
    explicit GeoDataPlacemark( const QString &name, GeoDataObject *parent = 0 );

    /**
    * Delete the placemark
    */
    ~GeoDataPlacemark();
    
    /**
    * Assignment operator
    */
    void operator=( const GeoDataPlacemark& other );
    
    /**
    * comparison operator is always wrong for now
    */
    bool operator==( const GeoDataPlacemark& other ) const { return false; };

    /**
     * Return the coordinate of the placemark as a GeoDataPoint
     */
    GeoDataCoordinates coordinate() const;

    /**
    * Return a pointer to the current Geometry object
    */
    GeoDataGeometry* geometry();

    /**
     * Return the coordinate of the placemark as @p longitude
     * and @p latitude.
     */
    void coordinate( qreal &longitude, qreal &latitude, qreal &altitude ) const;

    /**
     * Set the coordinate of the placemark in @p longitude and
     * @p latitude.
     */
    void setCoordinate( qreal longitude, qreal latitude, qreal altitude = 0 );
    
    /**
    * Set the coordinate of the placemark with an @p GeoDataPoint.
    */
    void setCoordinate( const GeoDataPoint &point );

    /**
    * Set any kind of @p GeoDataGeometry like @p GeoDataPoint , 
    * @p GeoDataLineString , @p GeoDataLinearRing , @p GeoDataMultiGeometry
    */
    void setGeometry( const GeoDataPoint& entry );
    void setGeometry( const GeoDataLineString& entry );
    void setGeometry( const GeoDataLinearRing& entry );
    void setGeometry( const GeoDataMultiGeometry& entry );
    void setGeometry( const GeoDataPolygon& entry );

    /**
     * Return the area size of the feature in square km.
     *
     * FIXME: Once we make Marble more area-aware we need to 
     * move this into the GeoDataArea class which will get 
     * inherited from GeoDataPlaceMark (or GeoDataFeature). 
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
     * Return the country code of the placemark.
     */
    const QString countryCode() const;

    /**
     * Set the country @p code of the placemark.
     */
    void setCountryCode( const QString &code );

    // Serialize the Placemark to @p stream
    virtual void pack( QDataStream& stream ) const;
    // Unserialize the Placemark from @p stream
    virtual void unpack( QDataStream& stream );

    virtual bool isPlacemark() const { return true; }

    virtual EnumFeatureId featureId() const { return GeoDataPlacemarkId; };

 private:
    GeoDataPlacemarkPrivate * const d;
};

}

#endif // GEODATAPLACEMARK_H
