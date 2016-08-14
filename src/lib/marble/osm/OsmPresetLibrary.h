//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2015    Marius-Valeriu Stanciu <stanciumarius94@gmail.com>
//

#ifndef MARBLE_OSMPRESETLIBRARY_H
#define MARBLE_OSMPRESETLIBRARY_H

#include <QMap>
#include <QPair>

#include "GeoDataFeature.h"
#include "marble_export.h"

namespace Marble
{
class GeoDataPlacemark;
class OsmPlacemarkData;

/**
 * @brief The OsmPresetLibrary class centralizes osm tag presets
 */
class MARBLE_EXPORT OsmPresetLibrary
{

public:
    typedef QPair<QString, QString> OsmTag;

    /**
     * @brief  Convenience categorization of placemarks for Osm key=value pairs
     */
    static GeoDataFeature::GeoDataVisualCategory osmVisualCategory(const OsmTag &tag);

    /**
     * @brief begin and end provide an stl style iterator for the preset map
     */
    static QMap<OsmTag, GeoDataFeature::GeoDataVisualCategory>::const_iterator begin();
    static QMap<OsmTag, GeoDataFeature::GeoDataVisualCategory>::const_iterator end();

    static QStringList shopValues();
    static QStringList buildingValues();

    static GeoDataFeature::GeoDataVisualCategory determineVisualCategory(const OsmPlacemarkData &osmData);

private:
    static void initializeOsmVisualCategories();

    /**
     * @brief s_visualCategories contains osm tag mappings to GeoDataVisualCategories
     */
    static QMap<OsmTag, GeoDataFeature::GeoDataVisualCategory> s_visualCategories;
};



}

#endif
