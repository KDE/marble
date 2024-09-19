// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2012 Dennis Nienhüser <nienhueser@kde.org>
//

#ifndef GEODATAGROUNDOVERLAY_P_H
#define GEODATAGROUNDOVERLAY_P_H

#include "GeoDataOverlay_p.h"

#include "GeoDataLatLonQuad.h"
#include "GeoDataTypes.h"

namespace Marble
{

class GeoDataGroundOverlayPrivate : public GeoDataOverlayPrivate
{
public:
    double m_altitude;

    AltitudeMode m_altitudeMode;

    GeoDataLatLonBox m_latLonBox;

    GeoDataLatLonQuad m_latLonQuad;

    GeoDataGroundOverlayPrivate();
};

GeoDataGroundOverlayPrivate::GeoDataGroundOverlayPrivate()
    : m_altitude(0.0)
    , m_altitudeMode(ClampToGround)
{
}

}

#endif
