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

#include "GeoDataContainer_p.h"
#include "GeoDataTypes.h"

namespace Marble
{

class GeoDataChangePrivate : public GeoDataContainerPrivate
{
};


GeoDataChange::GeoDataChange()
  : GeoDataContainer(new GeoDataChangePrivate)
{
}

GeoDataChange::GeoDataChange(const GeoDataChange &other)
  : GeoDataContainer(other, new GeoDataChangePrivate(*other.d_func()))
{
}

GeoDataChange &GeoDataChange::operator=( const GeoDataChange &other )
{
    if (this != &other) {
        Q_D(GeoDataChange);
        *d = *other.d_func();
    }

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
}

GeoDataFeature * GeoDataChange::clone() const
{
    return new GeoDataChange(*this);
}

const char *GeoDataChange::nodeType() const
{
    return GeoDataTypes::GeoDataChangeType;
}

}
