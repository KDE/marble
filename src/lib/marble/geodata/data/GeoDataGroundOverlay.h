// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2012 Dennis Nienhüser <nienhueser@kde.org>
//


#ifndef MARBLE_GEODATAGROUNDOVERLAY_H
#define MARBLE_GEODATAGROUNDOVERLAY_H

#include "GeoDataOverlay.h"
#include "MarbleGlobal.h"
#include "GeoDataLatLonBox.h"

namespace Marble {

class GeoDataGroundOverlayPrivate;
class GeoDataLatLonQuad;

/**
 */
class GEODATA_EXPORT GeoDataGroundOverlay: public GeoDataOverlay
{
public:
    GeoDataGroundOverlay();

    GeoDataGroundOverlay( const GeoDataGroundOverlay &other );

    ~GeoDataGroundOverlay() override;

    GeoDataGroundOverlay& operator=( const GeoDataGroundOverlay &other );
    bool operator==( const GeoDataGroundOverlay &other ) const;
    bool operator!=( const GeoDataGroundOverlay &other ) const;

    GeoDataFeature * clone() const override;

    /** Provides type information for downcasting a GeoNode */
    const char* nodeType() const override;

    double altitude() const;

    void setAltitude( double altitude );

    AltitudeMode altitudeMode() const;

    void setAltitudeMode( const AltitudeMode altitudeMode );

    const GeoDataLatLonBox& latLonBox() const;

    GeoDataLatLonBox& latLonBox();

    void setLatLonBox( const GeoDataLatLonBox &box );

    const GeoDataLatLonQuad& latLonQuad() const;

    GeoDataLatLonQuad& latLonQuad();

    void setLatLonQuad( const GeoDataLatLonQuad &quad );

private:
    Q_DECLARE_PRIVATE(GeoDataGroundOverlay)
};

}

#endif
