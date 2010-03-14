//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2008 Torsten Rahn <tackat@kde.org>
// Copyright 2008 Inge Wallin  <inge@lysator.liu.se>
//


#ifndef MARBLE_LAYER_INTERFACE_H
#define MARBLE_LAYER_INTERFACE_H

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
     * @return @c true  Returns whether the rendering has been successful
     */
    virtual bool render( GeoPainter *painter, ViewportParams *viewport,
       const QString& renderPos = "NONE", GeoSceneLayer * layer = 0 ) = 0;

};

} // namespace Marble

#endif // MARBLE_LAYER_INTERFACE_H
