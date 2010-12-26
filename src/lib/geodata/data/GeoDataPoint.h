//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2006-2007 Torsten Rahn <tackat@kde.org>
// Copyright 2007      Inge Wallin  <ingwa@kde.org>
// Copyright 2008      Patrick Spendrin <ps_ml@gmx.de>
//


#ifndef MARBLE_GEODATAPOINT_H
#define MARBLE_GEODATAPOINT_H

#include <QtCore/QMetaType>
#include <QtCore/QVector>

#include <cmath>

#include "geodata_export.h"
#include "GeoDataGeometry.h"
#include "GeoDataCoordinates.h"

namespace Marble
{


/** 
 * @short A Geometry object representing a 3d point
 *
 * GeoDataPoint is the GeoDataGeometry class representing a single three
 * dimensional point. It reflects the Point tag of KML spec and can be contained
 * in objects holding GeoDataGeometry objects. 
 * Nevertheless GeoDataPoint shouldn't be used if you just want to store 
 * 3d coordinates of a point that doesn't need to be inherited from GeoDataGeometry
 * In that case use GeoDataCoordinates instead which has nearly the same features 
 * and is much more light weight. 
 * Please consider this especially if you expect to have a high
 * amount of points e.g. for line strings, linear rings and polygons.
 * @see GeoDataCoordinates
 * @see GeoDataGeometry
*/

class GEODATA_EXPORT GeoDataPoint : public GeoDataCoordinates,
                                    public GeoDataGeometry
{
 public:
    typedef GeoDataCoordinates::Notation Notation;
    typedef GeoDataCoordinates::Unit Unit;

    GeoDataPoint( const GeoDataPoint& other );
    GeoDataPoint( const GeoDataCoordinates& other );
    GeoDataPoint( const GeoDataGeometry& other );
    GeoDataPoint();

    /**
     * @brief create a geopoint from longitude and latitude
     * @param _lon longitude
     * @param _lat latitude
     * @param alt altitude (default: 0)
     * @param _unit units that lon and lat get measured in
     * (default for Radian: north pole at pi/2, southpole at -pi/2)
     */
    GeoDataPoint( qreal _lon, qreal _lat, qreal alt = 0,
                  GeoDataPoint::Unit _unit = GeoDataPoint::Radian, int _detail = 0 );

    ~GeoDataPoint();

    /// Provides type information for downcasting a GeoData
    virtual const char* nodeType() const;

    // Type definitions
    typedef QVector<GeoDataPoint> Vector;


    // Serialize the Placemark to @p stream
    virtual void pack( QDataStream& stream ) const;
    // Unserialize the Placemark from @p stream
    virtual void unpack( QDataStream& stream );
    
    virtual void detach();
};

}

Q_DECLARE_METATYPE( Marble::GeoDataPoint )

#endif
