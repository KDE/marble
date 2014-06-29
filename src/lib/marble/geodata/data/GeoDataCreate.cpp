//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2014      Sanjiban Bairagya <sanjiban22393@gmail.com>
//

#include "GeoDataCreate.h"
#include "GeoDataTypes.h"

namespace Marble
{

class GeoDataCreatePrivate
{
public:
    GeoDataCreatePrivate();
};

GeoDataCreatePrivate::GeoDataCreatePrivate()
{
}

GeoDataCreate::GeoDataCreate() :
    d( new GeoDataCreatePrivate )
{
}

GeoDataCreate::GeoDataCreate( const Marble::GeoDataCreate &other ) :
    GeoDataContainer(), d( new GeoDataCreatePrivate( *other.d ) )
{
}

GeoDataCreate &GeoDataCreate::operator=( const GeoDataCreate &other )
{
    GeoDataContainer::operator =( other );
    *d = *other.d;
    return *this;
}

bool GeoDataCreate::operator==( const GeoDataCreate &other ) const
{
    if ( !GeoDataContainer::equals(other) ){
        return false;
    }
    return true;
}

bool GeoDataCreate::operator!=( const GeoDataCreate &other ) const
{
    return !this->operator==( other );
}

GeoDataCreate::~GeoDataCreate()
{
    delete d;
}

const char *GeoDataCreate::nodeType() const
{
    return GeoDataTypes::GeoDataCreateType;
}

}
