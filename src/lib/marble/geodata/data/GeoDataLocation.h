
//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2013      Sanjiban Bairagya <sanjiban22393@gmail.com>
//

#ifndef GEODATALOCATION_H
#define GEODATALOCATION_H

#include "GeoDataObject.h"
#include "GeoDataCoordinates.h"
#include "MarbleGlobal.h"

namespace Marble {

class GeoDataLocationPrivate;

/**
 */
class GEODATA_EXPORT GeoDataLocation: public GeoDataObject
{
public:
    GeoDataLocation();

    GeoDataLocation( const GeoDataLocation &other );

    GeoDataLocation& operator=( const GeoDataLocation &other );

    bool operator==( const GeoDataLocation &other ) const;
    bool operator!=( const GeoDataLocation &other ) const;

    ~GeoDataLocation();

    /** Provides type information for downcasting a GeoNode */
    virtual const char* nodeType() const;

    qreal latitude(GeoDataCoordinates::Unit) const;

    qreal longitude(GeoDataCoordinates::Unit) const;

    void setLatitude(qreal latitude, GeoDataCoordinates::Unit unit);

    void setLongitude(qreal longitude, GeoDataCoordinates::Unit unit);

    /** Returns the distance to earth's surface in meters, interpreted according to altitudeMode */
    qreal altitude() const;

    void setAltitude( qreal altitude );

protected:
    GeoDataLocationPrivate* const d;
};

}

#endif // GEODATAORIENTATION_H
