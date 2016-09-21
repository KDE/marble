//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011 Guillaume Martres <smarter@ubuntu.com>
//

#ifndef MARBLE_SATELLITESTLEITEM_H
#define MARBLE_SATELLITESTLEITEM_H

#include "TrackerPluginItem.h"

#include <sgp4unit.h>

class QColor;
class QDateTime;

namespace Marble {

class GeoDataCoordinates;
class GeoDataTrack;
class MarbleClock;

/**
 * An instance SatellitesTLEItem represents an item of a two-line-elements
 * set catalog.
 */
class SatellitesTLEItem : public TrackerPluginItem
{
public:
    SatellitesTLEItem( const QString &name,
                       elsetrec satrec,
                       const MarbleClock *clock );

    void update();

private:
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
    GeoDataCoordinates fromTEME( double x, double y, double z, double gmst ) const;

    /**
     * @return The time at the satellite epoch determined from m_satrec
     */
    QDateTime timeAtEpoch() const;

    /**
     * @return The orbital period of the satellite in seconds
     */
    double period() const;

    /**
     * @return The apogee of the satellite in km
     */
    double apogee() const;
    /**
     * @return The perigee of the satellite in km
     */
    double perigee() const;
    /**
     * @return The semi-major axis in km
     */
    double semiMajorAxis() const;

    /**
     * @return The inclination in degrees
     */
    double inclination() const;

    /**
     * Returns the Greenwich Mean Sideral Time in radians, @p minutes
     * after the epoch.
     */
    double gmst( double minutes ) const;

    /**
     * @return The square of @p x
     */
    static double square( double x );
};

} // namespace Marble

#endif // MARBLE_SATELLITESTLEITEM_H
