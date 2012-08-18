//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2012 Rene Kuettner <rene@bitkanal.net>
//

#ifndef MARBLE_PLANETARYSATELLITESITEM_H
#define MARBLE_PLANETARYSATELLITESITEM_H

// FIXME create an abstract class for other tracker plugins
#include "TrackerPluginItem.h"

#include "GeoDataCoordinates.h"
#include "GeoDataTrack.h"

#include "mex/planetarySats.h"

namespace Marble {

class GeoDataTrack;
class MarbleClock;
class GeoDataPlacemark;

class PlanetarySatellitesItem : public TrackerPluginItem
{
public:
    PlanetarySatellitesItem( const QString &name, PlanetarySats *planSat,
        const MarbleClock *clock );

    const QString name() { return m_name; }

    void update();
    void showOrbit( bool show );

private:
    bool m_showOrbit;
    GeoDataTrack *m_track;
    const MarbleClock *m_clock;
    PlanetarySats *m_planSat;
    const QString m_name;

    double m_perc;
    double m_apoc;
    double m_inc;
    double m_ecc;
    double m_ra;
    double m_tano;
    double m_m0;
    double m_a;
    double m_n0;

    void setDescription();
};

} // namespace Marble

#endif // MARBLE_PLANETARYSATELLITESITEM_H
