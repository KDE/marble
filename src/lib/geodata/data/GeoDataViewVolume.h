//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2013      Mohammed Nafees  <nafees.technocool@gmail.com>
//

#ifndef GEODATAVIEWVOLUME_H
#define GEODATAVIEWVOLUME_H

#include "GeoDataObject.h"
#include "geodata_export.h"

namespace Marble
{

class GeoDataViewVolumePrivate;

class GEODATA_EXPORT GeoDataViewVolume : public GeoDataObject
{
public:
    GeoDataViewVolume();

    GeoDataViewVolume( const GeoDataViewVolume &other );

    GeoDataViewVolume& operator=( const GeoDataViewVolume &other );

    ~GeoDataViewVolume();

    /** Provides type information for downcasting a GeoNode */
    virtual const char* nodeType() const;

    qreal leftFov() const;
    void setLeftFov( const qreal &leftFov );

    qreal rightFov() const;
    void setRightFov( const qreal &rightFov );

    qreal bottomFov() const;
    void setBottomFov( const qreal &bottomFov );

    qreal topFov() const;
    void setTopFov( const qreal &topFov );

    qreal near() const;
    void setNear( const qreal &near );

private:
    GeoDataViewVolumePrivate* const d;
};

}

#endif
