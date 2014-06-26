//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2014      Sanjiban Bairagya <sanjiban22393@gmail.com>
//

#include "GeoDataChange.h"
#include "GeoDataTypes.h"

namespace Marble
{

class GeoDataChangePrivate
{
public:
    GeoDataChangePrivate();
};

GeoDataChangePrivate::GeoDataChangePrivate()
{
}

GeoDataChange::GeoDataChange() :
    d( new GeoDataChangePrivate )
{
}

GeoDataChange::GeoDataChange( const Marble::GeoDataChange &other ) :
    GeoDataContainer(), d( new GeoDataChangePrivate( *other.d ) )
{
}

GeoDataChange &GeoDataChange::operator=( const GeoDataChange &other )
{
    GeoDataContainer::operator =( other );
    *d = *other.d;
    return *this;
}

bool GeoDataChange::operator==( const GeoDataChange &other ) const
{
    if ( !GeoDataContainer::equals(other) ){
        return false;
    }
    return true;
}

bool GeoDataChange::operator!=( const GeoDataChange &other ) const
{
    return !this->operator==( other );
}

GeoDataChange::~GeoDataChange()
{
    delete d;
}

const char *GeoDataChange::nodeType() const
{
    return GeoDataTypes::GeoDataChangeType;
}

}
