//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2013      Mayank Madan <maddiemadan@gmail.com>
//

#ifndef GEODATALATLONQUAD_H
#define GEODATALATLONQUAD_H

#include "GeoDataCoordinates.h"
#include "GeoDataObject.h"
#include "MarbleGlobal.h"

namespace Marble {

class GeoDataLatLonQuadPrivate;

/**
 */
class GEODATA_EXPORT GeoDataLatLonQuad: public GeoDataObject
{
public:
    GeoDataLatLonQuad();

    GeoDataLatLonQuad( const GeoDataLatLonQuad &other );

    GeoDataLatLonQuad& operator=( const GeoDataLatLonQuad &other );
    bool operator==( const GeoDataLatLonQuad &other ) const;
    bool operator!=( const GeoDataLatLonQuad &other ) const;

    ~GeoDataLatLonQuad();

    /** Provides type information for downcasting a GeoNode */
    virtual const char* nodeType() const;

    qreal bottomLeftLatitude( GeoDataCoordinates::Unit unit ) const;
    qreal bottomRightLatitude( GeoDataCoordinates::Unit unit ) const;
    qreal topRightLatitude( GeoDataCoordinates::Unit unit ) const;
    qreal topLeftLatitude( GeoDataCoordinates::Unit unit ) const;

    qreal bottomLeftLongitude( GeoDataCoordinates::Unit unit ) const;
    qreal bottomRightLongitude( GeoDataCoordinates::Unit unit ) const;
    qreal topRightLongitude( GeoDataCoordinates::Unit unit ) const;
    qreal topLeftLongitude( GeoDataCoordinates::Unit unit ) const;

    void setBottomLeftLatitude( qreal latitude, GeoDataCoordinates::Unit unit );
    void setBottomRightLatitude( qreal latitude, GeoDataCoordinates::Unit unit );
    void setTopRightLatitude( qreal latitude, GeoDataCoordinates::Unit unit );
    void setTopLeftLatitude( qreal latitude, GeoDataCoordinates::Unit unit );

    void setBottomLeftLongitude( qreal longitude, GeoDataCoordinates::Unit unit );
    void setBottomRightLongitude( qreal longitude, GeoDataCoordinates::Unit unit );
    void setTopRightLongitude( qreal longitude, GeoDataCoordinates::Unit unit );
    void setTopLeftLongitude( qreal longitude, GeoDataCoordinates::Unit unit );

    GeoDataCoordinates& bottomLeft() const;
    void setBottomLeft( const GeoDataCoordinates &coordinates );

    GeoDataCoordinates& bottomRight() const;
    void setBottomRight( const GeoDataCoordinates &coordinates );

    GeoDataCoordinates& topRight() const;
    void setTopRight( const GeoDataCoordinates &coordinates );

    GeoDataCoordinates& topLeft() const;
    void setTopLeft( const GeoDataCoordinates &coordinates );

    bool isValid() const;

private:
    GeoDataLatLonQuadPrivate* const d;
};

}

#endif
