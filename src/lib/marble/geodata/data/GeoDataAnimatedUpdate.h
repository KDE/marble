//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2014      Sanjiban Bairagya <sanjiban22393@gmail.com>
//


#ifndef MARBLE_GEODATAANIMATEDUPDATE_H
#define MARBLE_GEODATAANIMATEDUPDATE_H

#include "GeoDataTourPrimitive.h"
#include "MarbleGlobal.h"

namespace Marble {

class GeoDataAnimatedUpdatePrivate;
class GeoDataUpdate;

/**
 */
class GEODATA_EXPORT GeoDataAnimatedUpdate: public GeoDataTourPrimitive
{
public:

    GeoDataAnimatedUpdate();
    GeoDataAnimatedUpdate( const GeoDataAnimatedUpdate &other );
    GeoDataAnimatedUpdate& operator=( const GeoDataAnimatedUpdate &other );
    bool operator== (const GeoDataAnimatedUpdate &other) const;
    bool operator!= (const GeoDataAnimatedUpdate &other) const;
    ~GeoDataAnimatedUpdate();

    /** Provides type information for downcasting a GeoNode */
    virtual const char* nodeType() const;

    double duration() const;
    void setDuration( double duration );
    
    double delayedStart() const;
    void setDelayedStart( double delayedStart );

    const GeoDataUpdate* update() const;
    GeoDataUpdate* update();
    void setUpdate( GeoDataUpdate *update );

private:
    GeoDataAnimatedUpdatePrivate* const d;
};

}

#endif
