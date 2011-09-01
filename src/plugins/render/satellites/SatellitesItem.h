//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011 Guillaume Martres <smarter@ubuntu.com>
//

#ifndef MARBLE_SATELLITESITEM_H
#define MARBLE_SATELLITESITEM_H

#include "TrackerPluginItem.h"

#include "GeoDataCoordinates.h"

#include "sgp4/sgp4unit.h"

namespace Marble {

class SatellitesItem : public TrackerPluginItem
{
public:
    SatellitesItem( const QString &name, elsetrec satrec );

    void render( GeoPainter *painter, ViewportParams *viewport, const QString &renderPos, GeoSceneLayer *layer );

    void update();

private:
    double m_earthSemiMajorAxis; // in km
    elsetrec m_satrec;

    void setDescription();

    /**
     * Create a GeoDataCoordinates object from the cartesian coordinates
     * @p x, @p y and @p z in km in the Earth-centered inertial frame known
     * as TEME (True equator, Mean equinox) with Greenwich Mean Sidereal Time
     * @p gmst in radians at time of observation.
     */
    GeoDataCoordinates fromTEME( double x, double y, double z, double gmst );

    /**
     * @return The time since the epoch in minutes
     */
    double timeSinceEpoch();

    /**
     * @return The orbital period of the satellite in minutes
     */
    double period();

    /**
     * @return The apogee of the satellite in km
     */
    double apogee();
    /**
     * @return The perigee of the satellite in km
     */
    double perigee();
    /**
     * @return The semi-major axis in km
     */
    double semiMajorAxis();

    /**
     * @return The inclination in degrees
     */
    double inclination();

    /**
     * Returns The Greenwich Mean Sideral Time in radians, @p minutes
     * after the epoch.
     */
    double gmst( double minutes );

    /**
     * @return The square of @p x
     */
    double square( double x );
};

}

#endif // MARBLE_SATELLITESITEM_H
