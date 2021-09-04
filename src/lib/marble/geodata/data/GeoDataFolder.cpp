/*
    SPDX-FileCopyrightText: 2007 Murad Tagirov <tmurad@gmail.com>
    SPDX-FileCopyrightText: 2007 Nikolas Zimmermann <zimmermann@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "GeoDataFolder.h"

#include "GeoDataTypes.h"

#include "GeoDataContainer_p.h"

namespace Marble
{

class GeoDataFolderPrivate : public GeoDataContainerPrivate
{
};


GeoDataFolder::GeoDataFolder()
        : GeoDataContainer( new GeoDataFolderPrivate )
{
}

GeoDataFolder::GeoDataFolder( const GeoDataFolder& other )
    : GeoDataContainer(other, new GeoDataFolderPrivate(*other.d_func()))
{
}

GeoDataFolder::~GeoDataFolder()
{
}

GeoDataFolder& GeoDataFolder::operator=(const GeoDataFolder& other)
{
    if (this != &other) {
        Q_D(GeoDataFolder);
        *d = *other.d_func();
    }

    return *this;
}

bool GeoDataFolder::operator==( const GeoDataFolder &other ) const
{
    return GeoDataContainer::equals( other );
}

bool GeoDataFolder::operator!=( const GeoDataFolder &other ) const
{
    return !this->operator==( other );
}

const char* GeoDataFolder::nodeType() const
{
    return GeoDataTypes::GeoDataFolderType;
}

GeoDataFeature * GeoDataFolder::clone() const
{
    return new GeoDataFolder(*this);
}

}
