/*
    Copyright (C) 2007 Nikolas Zimmermann <zimmermann@kde.org>
    Copyright     2007 Murad Tagirov      <tmurad@gmail.com>
    Copyright     2007 Inge Wallin        <inge@lysator.liu.se>

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


#ifndef MARBLE_GEODATAFOLDER_H
#define MARBLE_GEODATAFOLDER_H


#include "GeoDataContainer.h"

#include "geodata_export.h"

namespace Marble
{

/**
 * @short A container that is used to arrange other GeoDataFeatures.
 *
 * A GeoDataFolder is used to arrange other GeoDataFeatures
 * hierarchically (Folders, Placemarks, NetworkLinks, or Overlays). A
 * GeoDataFeature is visible only if it and all its ancestors are
 * visible.
 *
 * @see GeoDataFeature
 * @see GeoDataContainer
 */

    class GeoDataFolderPrivate;

class GEODATA_EXPORT GeoDataFolder : public GeoDataContainer
{
 public:
    GeoDataFolder();
    GeoDataFolder( const GeoDataFolder& other );
    ~GeoDataFolder();

    /// Provides type information for downcasting a GeoData
    virtual QString nodeType() const;

 private:
    GeoDataFolderPrivate *p() const;
};

}

#endif
