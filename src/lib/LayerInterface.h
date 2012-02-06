//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2008 Torsten Rahn <tackat@kde.org>
// Copyright 2008 Inge Wallin  <inge@lysator.liu.se>
//

#ifndef MARBLE_LAYERINTERFACE_H
#define MARBLE_LAYERINTERFACE_H

#include "marble_export.h"

#include <QtCore/QStringList>

namespace Marble {

class GeoPainter;
class GeoSceneLayer;
class ViewportParams;

class MARBLE_EXPORT LayerInterface
{
public:

    /** Destructor */
    virtual ~LayerInterface();

    /**
     * @brief Preferred level in the layer stack for the rendering
     *
     * Gives a preferred level in the existing layer stack where
     * the render() method of this plugin should get executed.
     *.
     * Possible Values:
     * "NONE"
     * "STARS"
     * "BEHIND_TARGET"
     * "SURFACE"
     * "HOVERS_ABOVE_SURFACE"
     * "ATMOSPHERE"
     * "ORBIT"
     * "ALWAYS_ON_TOP"
     * "FLOAT_ITEM"
     * "USER_TOOLS"
     */
    virtual QStringList renderPosition() const = 0;

    /**
     * @brief Renders the content provided by the layer on the viewport.
     * @param painter  painter object allowing to paint on the map
     * @param viewport  metadata, such as current projection, screen dimension, etc.
     * @param renderPos  current render position
     * @param layer  deprecated, always zero (NULL)
     * @return @c true  Returns whether the rendering has been successful
     */
    virtual bool render( GeoPainter *painter, ViewportParams *viewport,
       const QString &renderPos, GeoSceneLayer *layer ) = 0;

    /**
      * @brief Returns the z value of the layer (default: 0.0). If two layers are painted
      * at the same render position, the one with the higher z value is painted on top.
      * If both have the same z value, their paint order is undefined.
      */
    virtual qreal zValue() const;
};

} // namespace Marble

#endif
