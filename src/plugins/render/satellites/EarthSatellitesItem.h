//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011 Guillaume Martres <smarter@ubuntu.com>
//

#ifndef MARBLE_EARTHSATELLITESITEM_H
#define MARBLE_EARTHSATELLITESITEM_H

#include "TrackerPluginItem.h"

#include "GeoDataCoordinates.h"
#include "GeoDataTrack.h"

#include "sgp4/sgp4unit.h"

namespace Marble {

class GeoDataTrack;
class MarbleClock;

class EarthSatellitesItem : public TrackerPluginItem
{

public:
    EarthSatellitesItem( const QString &name,
                         elsetrec satrec,
                         const MarbleClock *clock );

    void update();

    void showOrbit( bool show );

private:
    bool m_showOrbit;
    double m_earthSemiMajorAxis; // in km
    elsetrec m_satrec;

    GeoDataTrack *m_track;

    const MarbleClock *m_clock;

    void setDescription();

    /**
     * Add a point in the GeoDataTrack geometry of the placemark with time
     * dateTime and coordinates of the satellite determined from m_satrec.
     */
    void addPointAt( const QDateTime &dateTime );

    /**
     * Create a GeoDataCoordinates object from the cartesian coordinates
     * @p x, @p y and @p z in km in the Earth-centered inertial frame known
     * as TEME (True equator, Mean equinox) with Greenwich Mean Sidereal Time
     * @p gmst in radians at time of observation.
     */
    GeoDataCoordinates fromTEME( double x, double y, double z, double gmst );

    /**
     * @return The time at the satellite epoch determined from m_satrec
     */
    QDateTime timeAtEpoch();

    /**
     * @return The orbital period of the satellite in seconds
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
     * Returns the Greenwich Mean Sideral Time in radians, @p minutes
     * after the epoch.
     */
    double gmst( double minutes );

    /**
     * @return The square of @p x
     */
    double square( double x );
};

}

#endif // MARBLE_EARTHSATELLITESITEM_H
