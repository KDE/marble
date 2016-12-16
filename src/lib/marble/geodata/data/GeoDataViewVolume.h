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

/**
 */
class GEODATA_EXPORT GeoDataViewVolume : public GeoDataObject
{
public:
    GeoDataViewVolume();

    GeoDataViewVolume( const GeoDataViewVolume &other );

    GeoDataViewVolume& operator=( const GeoDataViewVolume &other );
    bool operator==( const GeoDataViewVolume &other ) const;
    bool operator!=( const GeoDataViewVolume &other ) const;

    ~GeoDataViewVolume() override;

    /** Provides type information for downcasting a GeoNode */
    const char* nodeType() const override;

    qreal leftFov() const;
    void setLeftFov(qreal leftFov);

    qreal rightFov() const;
    void setRightFov(qreal rightFov);

    qreal bottomFov() const;
    void setBottomFov(qreal bottomFov);

    qreal topFov() const;
    void setTopFov(qreal topFov);

    qreal near() const;
    void setNear(qreal near);

private:
    GeoDataViewVolumePrivate* const d;
};

}

#endif
