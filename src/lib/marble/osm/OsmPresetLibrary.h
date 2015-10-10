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
#include <QIcon>
#include <QSet>

#include "GeoDataFeature.h"
#include "marble_export.h"

namespace Marble
{
class GeoDataPlacemark;
class GeoDataStyle;
class OsmPlacemarkData;

/**
 * @brief The OsmPresetLibrary class centralizes osm tag presets
 */
class MARBLE_EXPORT OsmPresetLibrary
{

public:
    typedef QPair<QString, QString> OsmTag;

    /**
     * @brief corespondingIcon returns the icon that is associated with a visual category
     */
    static GeoDataStyle *presetStyle( const OsmTag &tag );

    /**
     * @brief  Convenience categorization of placemarks for Osm key=value pairs
     */
    static GeoDataFeature::GeoDataVisualCategory osmVisualCategory(const QString &keyValue );

    /**
     * @brief hasVisualCategory returns true if there is a visual category associated with
     * @p tag
     */
    static bool hasVisualCategory( const OsmTag &tag );

    /**
     * @brief begin and end provide an stl style iterator for the preset map
     */
    static QMap<OsmTag, GeoDataFeature::GeoDataVisualCategory>::const_iterator begin();
    static QMap<OsmTag, GeoDataFeature::GeoDataVisualCategory>::const_iterator end();

    /**
     * @brief additionalTagsBegin and end provide an stl style iterator for the additional tags map
     */
    static QList<OsmTag>::const_iterator additionalTagsBegin();
    static QList<OsmTag>::const_iterator additionalTagsEnd();

    static bool isAreaTag(const QString &keyValue);

    static QStringList buildingValues();

    static GeoDataFeature::GeoDataVisualCategory determineVisualCategory(const OsmPlacemarkData &osmData);

    static QList<GeoDataFeature::GeoDataVisualCategory> visualCategories(const OsmPlacemarkData &osmData);

private:
    static void initializeOsmVisualCategories();
    static void initializeAdditionalOsmTags();

    /**
     * @brief s_visualCategories contains osm tag mappings to GeoDataVisualCategories
     */
    static QMap<OsmTag, GeoDataFeature::GeoDataVisualCategory> s_visualCategories;

    /**
     * @brief s_additionalOsmTags is a list of useful, popular osm tags that currently don't have a visual category associated with them
     */
    static QList<OsmTag> s_additionalOsmTags;

    static QSet<QString> s_areaTags;
};



}

#endif
