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

#include <QSharedPointer>

#include <GeoDataStyle.h>
#include <GeoDataFeature.h>

namespace Marble {

class StyleParameters
{
public:
    explicit StyleParameters(const GeoDataFeature* feature=nullptr, int tileLevel=0);

    const GeoDataFeature* feature;
    int tileLevel;
};

class StyleBuilder
{
public:
    typedef QSharedPointer<StyleBuilder> Ptr;

    StyleBuilder();
    ~StyleBuilder();

    GeoDataStyle::ConstPtr createStyle(const StyleParameters &parameters) const;
    GeoDataStyle::ConstPtr presetStyle(GeoDataFeature::GeoDataVisualCategory visualCategory) const;

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
    int minimumZoomLevel(GeoDataFeature::GeoDataVisualCategory category) const;

    /**
     * @brief Returns the maximum zoom level in the theme.
     * @return maximum zoom level in the theme
     */
    int maximumZoomLevel() const;

    static QString visualCategoryName(GeoDataFeature::GeoDataVisualCategory category);

private:
    Q_DISABLE_COPY(StyleBuilder)

    class Private;
    Private * const d;
};

}

#endif
