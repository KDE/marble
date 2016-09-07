//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2016      Dennis Nienhüser <nienhueser@kde.org>
//

#ifndef MARBLE_STYLEBUILDER_H
#define MARBLE_STYLEBUILDER_H

#include "marble_export.h"

#include <GeoDataPlacemark.h>
#include <GeoDataStyle.h>

#include <QHash>
#include <QPair>

class QFont;
class QColor;

namespace Marble {
class OsmPlacemarkData;

class MARBLE_EXPORT StyleParameters
{
public:
    explicit StyleParameters(const GeoDataPlacemark *placemark_ = nullptr, int tileLevel = 0);

    const GeoDataPlacemark *placemark;
    int tileLevel;
};

class MARBLE_EXPORT StyleBuilder
{
public:
    typedef QPair<QString, QString> OsmTag;

    StyleBuilder();
    ~StyleBuilder();

    /**
     * Return the label font of the placemark.
     */
    QFont defaultFont() const;
    void setDefaultFont( const QFont& font );

    QColor defaultLabelColor() const;
    void setDefaultLabelColor( const QColor& color );

    GeoDataStyle::ConstPtr createStyle(const StyleParameters &parameters) const;

    /**
     * @brief Returns the order in which the visual categories used in the theme shall be painted on the map.
     * @return order in which the visual categories shall be painted on the map
     */
    QStringList renderOrder() const;

    void reset();

    /**
     * @brief Returns the zoom level from which on the given visual category will be visible.
     * @param category the visual category for which the minimum zoom level shall be returned
     * @return zoom level from which on the given visual category will be visible
     */
    int minimumZoomLevel(const GeoDataPlacemark &placemark) const;

    /**
     * @brief Returns the maximum zoom level in the theme.
     * @return maximum zoom level in the theme
     */
    int maximumZoomLevel() const;

    static QString visualCategoryName(GeoDataPlacemark::GeoDataVisualCategory category);

    /**
     * @brief begin and end provide an stl style iterator for the preset map
     */
    static QHash<OsmTag, GeoDataPlacemark::GeoDataVisualCategory>::const_iterator begin();
    static QHash<OsmTag, GeoDataPlacemark::GeoDataVisualCategory>::const_iterator end();

    static QStringList shopValues();
    static QSet<OsmTag> buildingTags();

    static GeoDataPlacemark::GeoDataVisualCategory determineVisualCategory(const OsmPlacemarkData &osmData);

private:
    Q_DISABLE_COPY(StyleBuilder)

    class Private;
    Private * const d;
};

}

#endif
