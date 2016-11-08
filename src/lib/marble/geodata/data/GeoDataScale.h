//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2013      Mayank Madan <maddiemadan@gmail.com>
// Copyright 2013      Sanjiban Bairagya <sanjiban22393@gmail.com>
//

#ifndef GEODATASCALE_H
#define GEODATASCALE_H

#include "GeoDataObject.h"
#include "MarbleGlobal.h"

namespace Marble {

class GeoDataScalePrivate;

/**
 */
class GEODATA_EXPORT GeoDataScale: public GeoDataObject
{
public:
    GeoDataScale();

    GeoDataScale( const GeoDataScale &other );

    GeoDataScale& operator=( const GeoDataScale &other );

    bool operator==( const GeoDataScale &other ) const;
    bool operator!=( const GeoDataScale &other ) const;

    ~GeoDataScale();

    /** Provides type information for downcasting a GeoNode */
    virtual const char* nodeType() const;

    double x() const;

    void setX( double x );

    double y() const;

    void setY( double y );

    double z() const;

    void setZ( double z);

private:
    GeoDataScalePrivate* const d;
};

}

#endif // GEODATASCALE_H
