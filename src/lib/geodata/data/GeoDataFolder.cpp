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
  public:
    GeoDataFolderPrivate()
    {
    }

    virtual void* copy()
    {
        GeoDataFolderPrivate* copy = new GeoDataFolderPrivate;
        *copy = *this;
        return copy;
    }

    virtual const char* nodeType() const
    {
        return GeoDataTypes::GeoDataFolderType;
    }
};


GeoDataFolder::GeoDataFolder()
        : GeoDataContainer( new GeoDataFolderPrivate )
{
}

GeoDataFolder::GeoDataFolder( const GeoDataFolder& other )
    : GeoDataContainer( other )
{
}

GeoDataFolder::~GeoDataFolder()
{
}

GeoDataFolderPrivate* GeoDataFolder::p() const
{
    return static_cast<GeoDataFolderPrivate*>(d);
}

const char* GeoDataFolder::nodeType() const
{
    return p()->nodeType();
}

}
