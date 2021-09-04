// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2007 Murad Tagirov <tmurad@gmail.com>
//


#include "GeoDataStyleSelector.h"
#include <QDataStream>

#include "GeoDataTypes.h"

namespace Marble
{

class GeoDataStyleSelectorPrivate
{
};

GeoDataStyleSelector::GeoDataStyleSelector() :
    GeoDataObject(),
    d( nullptr )
{
}

GeoDataStyleSelector::GeoDataStyleSelector( const GeoDataStyleSelector& other ) :
    GeoDataObject( other ),
    d( nullptr )
{
}

GeoDataStyleSelector::~GeoDataStyleSelector()
{
    delete d;
}

GeoDataStyleSelector& GeoDataStyleSelector::operator=( const GeoDataStyleSelector& other )
{
    GeoDataObject::operator=( other );
    return *this;
}

bool GeoDataStyleSelector::operator==( const GeoDataStyleSelector &other ) const
{
    return GeoDataObject::equals( other );
}

bool GeoDataStyleSelector::operator!=( const GeoDataStyleSelector &other ) const
{
    return !this->operator==( other );
}

void GeoDataStyleSelector::pack( QDataStream& stream ) const
{
    GeoDataObject::pack( stream );
}

void GeoDataStyleSelector::unpack( QDataStream& stream )
{
    GeoDataObject::unpack( stream );
}

}
