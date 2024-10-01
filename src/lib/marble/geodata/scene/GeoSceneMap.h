/*
    SPDX-FileCopyrightText: 2008 Torsten Rahn <rahn@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef MARBLE_GEOSCENEMAP_H
#define MARBLE_GEOSCENEMAP_H

#include <QList>
#include <QVariant>

#include <geodata_export.h>

#include "GeoDocument.h"

class QColor;
class QString;

namespace Marble
{

class GeoSceneLayer;
class GeoSceneFilter;

class GeoSceneMapPrivate;

/**
 * @short Map layer structure of a GeoScene document.
 */
class GEODATA_EXPORT GeoSceneMap : public GeoNode
{
public:
    GeoSceneMap();
    ~GeoSceneMap() override;
    const char *nodeType() const override;

    QColor backgroundColor() const;
    void setBackgroundColor(const QColor &);

    QColor labelColor() const;
    void setLabelColor(const QColor &);

    QColor highlightBrushColor() const;
    void setHighlightBrushColor(const QColor &);

    QColor highlightPenColor() const;
    void setHighlightPenColor(const QColor &);
    /**
     * @brief  Add a new layer to the map
     * @param  layer  The new layer
     */
    void addLayer(GeoSceneLayer *);
    /**
     * @ brief Set starting center with lon lat cooredinates
     * used if a scene downloadUrl do not handle elements in other locations
     */
    void setCenter(const QString &coordinateString);
    /**
     * @breif Get starting center with cooredinates
     * used if a scene downloadUrl do not handle elements in other locations
     * return A QVariantList of lon lat as specified in the dgml
     */
    QVariantList center() const;
    /**
     * @brief  Return a layer by its name
     * @param  name  The name of the layer
     * @return A pointer to the layer request by its name
     */
    GeoSceneLayer *layer(const QString &name);
    const GeoSceneLayer *layer(const QString &name) const;

    /**
     * @brief  Return all layers
     * @return A vector that contains pointers to all available layers
     */
    QList<GeoSceneLayer *> layers() const;

    /**
     * @brief  Add a new filter to the map
     * @param  filter  The new filter
     */
    void addFilter(GeoSceneFilter *);

    /**
     * @brief  Return a filter by its name
     * @param  name  The name of the filter
     * @return A pointer to the filter request by its name
     */
    GeoSceneFilter *filter(const QString &name);

    /**
     * @brief  Return all filters
     * @return A vector that contains pointers to all available filters
     */
    QList<GeoSceneFilter *> filters() const;

    /**
     * @brief  Checks for valid layers that contain texture data
     * @return Whether a texture layer got created internally
     *
     * NOTE: The existence of the file(s) that contain the actual data
     *       still needs to get checked at runtime!
     */
    bool hasTextureLayers() const;

    /**
     * @brief  Checks for valid layers that contain vector data
     * @return Whether a vector layer got created internally
     *
     * NOTE: The existence of the file(s) that contain the actual data
     *       still needs to get checked at runtime!
     */
    bool hasVectorLayers() const;

private:
    Q_DISABLE_COPY(GeoSceneMap)
    GeoSceneMapPrivate *const d;
};

}

#endif
