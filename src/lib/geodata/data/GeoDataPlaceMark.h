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
 * This class represents a place mark, e.g. a city or a mountain.
 * It is filled with data by the GeoData or GPX loader and the
 * PlaceMarkModel makes use of it.
 */
class GeoDataPlaceMark: public GeoDataFeature
{
 public:
    /**
     * Creates a new place mark.
     */
    GeoDataPlaceMark();

    /**
     * Creates a new place mark with the given @p name.
     */
    GeoDataPlaceMark( const QString &name );

    /**
     * Returns the coordinat of the place mark as GeoPoint
     * object.
     */
    GeoPoint coordinate() const;

    /**
     * Returns the coordinate of the place mark as @p longitude
     * and @p latitude.
     */
    void coordinate( double &longitude, double &latitude );

    /**
     * Sets the coordinate of the place mark in @p longitude and
     * @p latitude.
     */
    void setCoordinate( double longitude, double latitude );

    /**
     * Returns the country code of the place mark.
     */
    const QString countryCode() const;

    /**
     * Sets the country @p code of the place mark.
     */
    void setCountryCode( const QString &code );

    /**
     * Serializable methods
     */
    virtual void pack( QDataStream& stream ) const;
    virtual void unpack( QDataStream& stream );

 private:
    // Basic data
    GeoPoint m_coordinate;
    QString m_countrycode;
};

#endif // GEODATAPLACEMARK_H
