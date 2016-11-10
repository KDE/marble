//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2012      Dennis Nienhüser <nienhueser@kde.org>
//

#ifndef GEODATAGROUNDOVERLAY_P_H
#define GEODATAGROUNDOVERLAY_P_H

#include "GeoDataOverlay_p.h"

#include "GeoDataTypes.h"
#include "GeoDataLatLonQuad.h"

namespace Marble {

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
    : m_altitude(0.0),
      m_altitudeMode(ClampToGround)
{
}

}

#endif
