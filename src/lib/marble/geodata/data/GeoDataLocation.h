
// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2013 Sanjiban Bairagya <sanjiban22393@gmail.com>
//

#ifndef GEODATALOCATION_H
#define GEODATALOCATION_H

#include "GeoDataCoordinates.h"
#include "GeoDataObject.h"
#include "MarbleGlobal.h"

namespace Marble
{

class GeoDataLocationPrivate;

/**
 */
class GEODATA_EXPORT GeoDataLocation : public GeoDataObject
{
public:
    GeoDataLocation();

    GeoDataLocation(const GeoDataLocation &other);

    GeoDataLocation &operator=(const GeoDataLocation &other);

    bool operator==(const GeoDataLocation &other) const;
    bool operator!=(const GeoDataLocation &other) const;

    ~GeoDataLocation() override;

    /** Provides type information for downcasting a GeoNode */
    const char *nodeType() const override;

    qreal latitude(GeoDataCoordinates::Unit) const;

    qreal longitude(GeoDataCoordinates::Unit) const;

    void setLatitude(qreal latitude, GeoDataCoordinates::Unit unit);

    void setLongitude(qreal longitude, GeoDataCoordinates::Unit unit);

    /** Returns the distance to earth's surface in meters, interpreted according to altitudeMode */
    qreal altitude() const;

    void setAltitude(qreal altitude);

protected:
    GeoDataLocationPrivate *const d;
};

}

#endif // GEODATAORIENTATION_H
