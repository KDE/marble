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


#ifndef PLACEMARK_H
#define PLACEMARK_H

#include <QtCore/QChar>

#include "GeoPoint.h"
#include "kml/KMLFeature.h"

class QPixmap;
class QSize;

/**
 * This class represents a place mark, e.g. a city or a mountain.
 * It is filled with data by the KML or GPX loader and the
 * PlaceMarkModel makes use of it.
 */
class PlaceMark : public KMLFeature
{
 public:
    /**
     * Creates a new place mark.
     */
    PlaceMark();

    /**
     * Creates a new place mark with the given @p name.
     */
    PlaceMark( const QString &name );

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
     * Returns the role of the place mark.
     *
     * TODO: describe roles here!
     */
    const QChar role() const;

    /**
     * Sets the role of the place mark.
     */
    void setRole( const QChar &role );

    /**
     * Returns the country code of the place mark.
     */
    const QString countryCode() const;

    /**
     * Sets the country @p code of the place mark.
     */
    void setCountryCode( const QString &code );

    /**
     * Returns the symbol index of the place mark.
     */
    const int symbolIndex() const; 

    /**
     * Sets the symbol @p index of the place mark.
     */
    void setSymbolIndex( int index );

    /**
     * Returns the population index of the place mark.
     *
     * The population index is a value which describes
     * at which zoom level the place mark will be shown.
     */
    const int populationIndex() const;

    /**
     * Sets the population @p index of the place mark.
     */
    void setPopulationIndex( int index );

    /**
     * Returns the population of the place mark.
     */
    const int population() const;

    /**
     * Sets the @p population of the place mark.
     */
    void setPopulation( int population );

    /**
     * Returns the symbol size of the place mark.
     */
    const QSize symbolSize() const;

    /**
     * Returns the symbol of the place mark.
     */
    const QPixmap symbolPixmap() const;

    /*
     * Serializable methods
     */
    virtual void pack( QDataStream& stream ) const;
    virtual void unpack( QDataStream& stream );

 private:
    // Basic data
    GeoPoint m_coordinate;
    QChar m_role;
    QString m_countrycode;
    int m_population;

    // View specific data
    int m_symbolIndex;
    int m_populationIndex;
};

#endif // PLACEMARK_H
