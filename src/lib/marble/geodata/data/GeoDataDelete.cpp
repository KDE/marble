// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2014 Sanjiban Bairagya <sanjiban22393@gmail.com>
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
    return GeoDataContainer::equals(other);
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
