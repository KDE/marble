/*
    SPDX-FileCopyrightText: 2007 Nikolas Zimmermann <zimmermann@kde.org>
    SPDX-FileCopyrightText: 2007 Murad Tagirov <tmurad@gmail.com>
    SPDX-FileCopyrightText: 2007 Inge Wallin <inge@lysator.liu.se>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef MARBLE_GEODATAFOLDER_H
#define MARBLE_GEODATAFOLDER_H


#include "GeoDataContainer.h"

#include "geodata_export.h"

namespace Marble
{

class GeoDataFolderPrivate;

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
class GEODATA_EXPORT GeoDataFolder : public GeoDataContainer
{
 public:
    GeoDataFolder();
    GeoDataFolder( const GeoDataFolder& other );
    ~GeoDataFolder() override;

    GeoDataFolder& operator=(const GeoDataFolder& other);

    bool operator==( const GeoDataFolder &other ) const;
    bool operator!=( const GeoDataFolder &other ) const;

    const char* nodeType() const override;

    GeoDataFeature * clone() const override;

 private:
    Q_DECLARE_PRIVATE(GeoDataFolder)
};

}

Q_DECLARE_METATYPE(Marble::GeoDataFolder*)

#endif
