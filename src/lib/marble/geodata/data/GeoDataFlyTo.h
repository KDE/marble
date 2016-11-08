//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2013      Mayank Madan <maddiemadan@gmail.com>
//


#ifndef MARBLE_GEODATAFLYTO_H
#define MARBLE_GEODATAFLYTO_H

#include "GeoDataTourPrimitive.h"
#include "MarbleGlobal.h"

namespace Marble {

class GeoDataAbstractView;
class GeoDataFlyToPrivate;

/**
 */
class GEODATA_EXPORT GeoDataFlyTo: public GeoDataTourPrimitive
{
public:
    enum FlyToMode {
        Bounce,
        Smooth
    };

    GeoDataFlyTo();

    GeoDataFlyTo( const GeoDataFlyTo &other );

    GeoDataFlyTo& operator=( const GeoDataFlyTo &other );
    bool operator==( const GeoDataFlyTo& other ) const;
    bool operator!=( const GeoDataFlyTo& other ) const;

    ~GeoDataFlyTo();

    /** Provides type information for downcasting a GeoNode */
    virtual const char* nodeType() const;

    double duration() const;

    void setDuration( double duration );

    const GeoDataAbstractView *view() const;

    GeoDataAbstractView *view();

    void setView( GeoDataAbstractView *view );

    FlyToMode flyToMode() const;

    void setFlyToMode( const FlyToMode flyToMode );

private:
    GeoDataFlyToPrivate* const d;
};

}

#endif
