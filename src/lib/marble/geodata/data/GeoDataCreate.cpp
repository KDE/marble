// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2014 Sanjiban Bairagya <sanjiban22393@gmail.com>
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
    return GeoDataContainer::equals(other);
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
