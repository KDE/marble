//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2006-2007 Torsten Rahn <tackat@kde.org>"
// Copyright 2007      Inge Wallin  <ingwa@kde.org>"
//


#ifndef GEODATAPLACEMARK_H
#define GEODATAPLACEMARK_H


#include <QtCore/QChar>

#include "GeoPoint.h"
#include "GeoDataFeature.h"

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

class GeoDataPlacemark: public GeoDataFeature
{
 public:
    /**
     * Create a new placemark.
     */
    GeoDataPlacemark();

    /**
     * Create a new placemark with the given @p name.
     */
    GeoDataPlacemark( const QString &name );

    /**
     * Return the coordinate of the placemark as a GeoPoint
     */
    GeoPoint coordinate() const;

    /**
     * Return the coordinate of the placemark as @p longitude
     * and @p latitude.
     */
    void coordinate( double &longitude, double &latitude );

    /**
     * Set the coordinate of the placemark in @p longitude and
     * @p latitude.
     */
    void setCoordinate( double longitude, double latitude );

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

 private:
    // Data for a Placemark in addition to those in GeoDataFeature.
    GeoPoint  m_coordinate;     // The geographic position
    QString   m_countrycode;    // Country code.
};

#endif // GEODATAPLACEMARK_H
