/*
    Copyright (C) 2007 Murad Tagirov <tmurad@gmail.com>
    Copyright (C) 2007 Nikolas Zimmermann <zimmermann@kde.org>

    This file is part of the KDE project

    This library is free software you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    aint with this library see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
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
