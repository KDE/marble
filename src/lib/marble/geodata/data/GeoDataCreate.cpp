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

#include "GeoDataContainer_p.h"
#include "GeoDataTypes.h"

namespace Marble
{

class GeoDataCreatePrivate : public GeoDataContainerPrivate
{
};


GeoDataCreate::GeoDataCreate()
  : GeoDataContainer(new GeoDataCreatePrivate)
{
}

GeoDataCreate::GeoDataCreate(const GeoDataCreate &other)
  : GeoDataContainer(other, new GeoDataCreatePrivate(*other.d_func()))
{
}

GeoDataCreate &GeoDataCreate::operator=( const GeoDataCreate &other )
{
    if (this != &other) {
        Q_D(GeoDataCreate);
        *d = *other.d_func();
    }

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
}

GeoDataFeature * GeoDataCreate::clone() const
{
    return new GeoDataCreate(*this);
}

const char *GeoDataCreate::nodeType() const
{
    return GeoDataTypes::GeoDataCreateType;
}

}
