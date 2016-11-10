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

#include "GeoDataContainer_p.h"
#include "GeoDataTypes.h"

namespace Marble
{

class GeoDataDeletePrivate : public GeoDataContainerPrivate
{
};


GeoDataDelete::GeoDataDelete()
  : GeoDataContainer(new GeoDataDeletePrivate)
{
}

GeoDataDelete::GeoDataDelete(const GeoDataDelete &other)
  : GeoDataContainer(other, new GeoDataDeletePrivate(*other.d_func()))
{
}

GeoDataDelete &GeoDataDelete::operator=( const GeoDataDelete &other )
{
    if (this != &other) {
        Q_D(GeoDataDelete);
        *d = *other.d_func();
    }

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
}

GeoDataFeature * GeoDataDelete::clone() const
{
    return new GeoDataDelete(*this);
}

const char *GeoDataDelete::nodeType() const
{
    return GeoDataTypes::GeoDataDeleteType;
}

}
