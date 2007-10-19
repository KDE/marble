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

#include <QtCore/QSize>
#include <QtGui/QPixmap>
#include <QtGui/QFont>
#include <QtGui/QColor>

class GeoDataStyle;

/**
 * This class represents a place mark, e.g. a city or a mountain.
 * It is filled with data by the GeoData or GPX loader and the
 * PlaceMarkModel makes use of it.
 */
class GeoDataPlaceMark: public GeoDataFeature
{
 public:

    enum SymbolIndex{  Empty,
                       Default,
                       Unknown,
                       SmallCity,           // The order of the cities needs 
                       SmallCountyCapital,  // to stay fixed as the algorithms
                       SmallStateCapital,   // rely on that.
                       SmallNationCapital,
                       MediumCity,
                       MediumCountyCapital,
                       MediumStateCapital,
                       MediumNationCapital,
                       BigCity,
                       BigCountyCapital,
                       BigStateCapital,
                       BigNationCapital,
                       LargeCity,
                       LargeCountyCapital,
                       LargeStateCapital,
                       LargeNationCapital,
                       Mountain,
                       Volcano,
                       GeographicPole,
                       MagneticPole,
                       ShipWreck,
                       AirPort,
                       LastIndex // Important: Make sure that this is always the last 
                                 // item and just use it to specify the array size
                   };

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
    const GeoDataPlaceMark::SymbolIndex symbolIndex() const; 

    /**
     * Sets the symbol @p index of the place mark.
     */
    void setSymbolIndex( GeoDataPlaceMark::SymbolIndex index );

    /**
     * Returns the popularity index of the place mark.
     *
     * The popularity index is a value which describes
     * at which zoom level the place mark will be shown.
     */
    const int popularityIndex() const;

    /**
     * Sets the popularity @p index of the place mark.
     */
    void setPopularityIndex( int index );

    /**
     * Returns the popularity of the place mark.
     */
    const qint64 popularity() const;

    /**
     * Sets the @p popularity of the place mark.
     */
    void setPopularity( qint64 popularity );

    /**
     * Returns the symbol size of the place mark.
     */
    const QSize symbolSize() const;

    /**
     * Returns the symbol of the place mark.
     */
    const QPixmap symbolPixmap() const;

    /**
     * Returns the label color of the place mark.
     */
    static const QColor labelColor( int id );

    /**
     * Returns the label font of the place mark.
     */
    static const QFont labelFont( int id );

    /**
     * Returns the label font of the place mark.
     */
    static void resetDefaultStyle();

    /**
     * Serializable methods
     */
    virtual void pack( QDataStream& stream ) const;
    virtual void unpack( QDataStream& stream );

 private:
    // Basic data
    GeoPoint m_coordinate;
    QChar m_role;
    QString m_countrycode;
    qint64 m_popularity;

    // View specific data
    GeoDataStyle* m_style;

    SymbolIndex m_symbolIndex;

    int m_popularityIndex;

    static QPixmap* s_placeSymbol[GeoDataPlaceMark::LastIndex];
    static QSize    s_placeSize[GeoDataPlaceMark::LastIndex];
    static QFont    s_placeLabelFont[GeoDataPlaceMark::LastIndex];
    static QColor   s_placeLabelColor[GeoDataPlaceMark::LastIndex];

    static bool s_defaultStyleInitialized;
    static void initializeDefaultStyle();
};

#endif // GEODATAPLACEMARK_H
