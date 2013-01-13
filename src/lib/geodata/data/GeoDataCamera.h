//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2013      Mayank Madan <maddiemadan@gmail.com>
//

#ifndef MARBLE_GEODATACAMERA_H
#define MARBLE_GEODATACAMERA_H

#include "geodata_export.h"
#include "GeoDataAbstractView.h"
#include "GeoDataCoordinates.h"

namespace Marble
{
class GeoDataCameraPrivate;

class GEODATA_EXPORT GeoDataCamera : public GeoDataAbstractView
{
public:
    GeoDataCamera();

    GeoDataCamera(const GeoDataCamera& other);

    GeoDataCamera& operator=(const GeoDataCamera &other);

    ~GeoDataCamera();

    /**
     * @brief set the altitude in a GeoDataCamera object
     * @param altitude latitude
     *
     */
    void setAltitude( qreal altitude);

    /**
     * @brief retrieves the altitude of the GeoDataCamera object
     * @return latitude
     */
    qreal altitude( ) const;

    /**
     * @brief set the latitude in a GeoDataCamera object
     * @param latitude latitude
     * @param unit units that lon and lat get measured in
     * (default for Radian: north pole at pi/2, southpole at -pi/2)
     */
    void setLatitude( qreal latitude,GeoDataCoordinates::Unit unit = GeoDataCoordinates::Radian );

    /**
     * @brief retrieves the latitude of the GeoDataCamera object
     * use the unit parameter to switch between Radian and DMS
     * @param unit units that lon and lat get measured in
     * (default for Radian: north pole at pi/2, southpole at -pi/2)
     * @return latitude
     */
    qreal latitude( GeoDataCoordinates::Unit unit = GeoDataCoordinates::Radian ) const;

    /**
     * @brief set the longitude in a GeoDataCamera object
     * @param longitude longitude
     * @param unit units that lon and lat get measured in
     * (default for Radian: north pole at pi/2, southpole at -pi/2)
     */
    void setLongitude( qreal longitude,GeoDataCoordinates::Unit unit = GeoDataCoordinates::Radian );

    /**
     * @brief retrieves the longitude of the GeoDataCamera object
     * use the unit parameter to switch between Radian and DMS
     * @param unit units that lon and lat get measured in
     * (default for Radian: north pole at pi/2, southpole at -pi/2)
     * @return latitude
     */
    qreal longitude( GeoDataCoordinates::Unit unit = GeoDataCoordinates::Radian ) const;

    /**
     * @brief retrieve the lat/lon/alt triple as a GeoDataCoordinates object
     * @return GeoDataCoordinates
     * @see longitude latitude altitude
     */
    GeoDataCoordinates coordinates() const;

    void setRoll( qreal roll );

    qreal roll() const;

    qreal heading() const;

    void setHeading(qreal heading);

    qreal tilt() const;

    void setTilt(qreal tilt);

    AltitudeMode altitudeMode() const;

    void setAltitudeMode(const AltitudeMode altitudeMode);

    void setCoordinates( const GeoDataCoordinates& coordinates );

    /// Provides type information for downcasting a GeoNode
    virtual const char* nodeType() const;

    void detach();
private:
    GeoDataCameraPrivate *d;

};

}

Q_DECLARE_METATYPE( Marble::GeoDataCamera )

#endif
