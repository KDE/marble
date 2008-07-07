//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2006-2007 Torsten Rahn <tackat@kde.org>"
// Copyright 2007      Inge Wallin  <ingwa@kde.org>"
// Copyright 2008      Patrick Spendrin <ps_ml@gmx.de>"
//


#ifndef GEODATACOORDINATES_H
#define GEODATACOORDINATES_H

#include <QtCore/QMetaType>
#include <QtCore/QVector>
#include <QtCore/QString>

#include <cmath>

#include "geodata_export.h"
#include "Quaternion.h"
#include "GeoDataObject.h"

/* M_PI is a #define that may or may not be handled in <cmath> */
#ifndef M_PI 
#define M_PI 3.14159265358979323846264338327950288419717
#endif

const double TWOPI = 2 * M_PI;

class GeoDataCoordinatesPrivate;

class GEODATA_EXPORT GeoDataCoordinates : public GeoDataObject {
 public:
    /**
     * @brief enum used constructor to specify the units used
     *
     * Internally we always use radian for mathematical convenience.
     * However the Marble's interfaces to the outside should default 
     * to degrees.
     */
    enum Unit{
        Radian,
        Degree
    };
    /**
     * @brief enum used to specify the notation / numerical system
     *
     * For degrees there exist two notations: 
     * "Decimal" (base-10) and the "Sexagesimal DMS" (base-60) which is 
     * traditionally used in cartography. Decimal notation 
     * uses floating point numbers to specify parts of a degree. The 
     * Sexagesimal DMS notation uses integer based 
     * Degrees-(Arc)Minutes-(Arc)Seconds to describe parts of a degree. 
     */
    enum Notation{
        Decimal,
        DMS
    };

    GeoDataCoordinates( const GeoDataCoordinates& other );
    GeoDataCoordinates();

    /**
     * @brief create a geocoordinate from longitude and latitude
     * @param _lon longitude
     * @param _lat latitude
     * @param alt altitude (default: 0)
     * @param _unit units that lon and lat get measured in
     * (default for Radian: north pole at pi/2, southpole at -pi/2)
     */
    GeoDataCoordinates( double _lon, double _lat, double alt = 0,
             GeoDataCoordinates::Unit _unit = GeoDataCoordinates::Radian );

    ~GeoDataCoordinates();

    double altitude() const;
    void   setAltitude( const double altitude );

    void set(double _lon, double _lat, double alt = 0,
             GeoDataCoordinates::Unit _unit = GeoDataCoordinates::Radian );

    void geoCoordinates( double& lon, double& lat, 
                         GeoDataCoordinates::Unit unit = GeoDataCoordinates::Radian )
                                                                const;

    const Quaternion &quaternion() const;

    QString toString( GeoDataCoordinates::Notation notation = GeoDataCoordinates::DMS );
    bool operator==( const GeoDataCoordinates& ) const;
    GeoDataCoordinates& operator=( const GeoDataCoordinates &other );

    /// Serialize the contents of the feature to @p stream.
    virtual void pack( QDataStream& stream ) const;
    /// Unserialize the contents of the feature from @p stream.
    virtual void unpack( QDataStream& stream );

 protected:
    GeoDataCoordinatesPrivate* const d_ptr;

};

Q_DECLARE_METATYPE( GeoDataCoordinates )

#endif // GEODATACOORDINATES_H
