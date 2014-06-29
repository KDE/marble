//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2014      Sanjiban Bairagya <sanjiban22393@gmail.com>
//

#include "GeoDataDelete.h"
#include "GeoDataTypes.h"

namespace Marble
{

class GeoDataDeletePrivate
{
public:
    GeoDataDeletePrivate();
};

GeoDataDeletePrivate::GeoDataDeletePrivate()
{
}

GeoDataDelete::GeoDataDelete() :
    d( new GeoDataDeletePrivate )
{
}

GeoDataDelete::GeoDataDelete( const Marble::GeoDataDelete &other ) :
    GeoDataContainer(), d( new GeoDataDeletePrivate( *other.d ) )
{
}

GeoDataDelete &GeoDataDelete::operator=( const GeoDataDelete &other )
{
    GeoDataContainer::operator =( other );
    *d = *other.d;
    return *this;
}

bool GeoDataDelete::operator==( const GeoDataDelete &other ) const
{
    if ( !GeoDataContainer::equals(other) ){
        return false;
    }
    return true;
}

bool GeoDataDelete::operator!=( const GeoDataDelete &other ) const
{
    return !this->operator==( other );
}

GeoDataDelete::~GeoDataDelete()
{
    delete d;
}

const char *GeoDataDelete::nodeType() const
{
    return GeoDataTypes::GeoDataDeleteType;
}

}
