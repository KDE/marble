//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2008 Torsten Rahn <tackat@kde.org>"
// Copyright 2008 Inge Wallin  <inge@lysator.liu.se>"
//


#ifndef MARBLERENDERPLUGININTERFACEH
#define MARBLERENDERPLUGININTERFACEH

#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QtPlugin>

#include "PluginInterface.h"

namespace Marble
{

class GeoPainter;
class GeoSceneLayer;
class ViewportParams;
class RenderPlugin;

/**
 * @short The class that specifies the Marble layer interface of a plugin.
 *
 */

class RenderPluginInterface: public PluginInterface
{
 public:
    virtual ~RenderPluginInterface();
    
    /**
    * @brief Returns a new object of the plugin
    */
    virtual RenderPlugin* pluginInstance() = 0;

    /**
     * @brief Returns the name(s) of the backend that the plugin can render
     *.
     * This method should return the name of the backend that the plugin
     * can render. The string has to be the same one that is given
     * for the attribute in the layer element of the DGML file that
     * backend is able to process.
     * Examples to replace available default backends would be "vector" or
     * "texture". To provide a completely new kind of functionality please
     * choose your own unique string.
     */
    virtual QStringList backendTypes() const = 0;

    /**
     * @brief Return how the plugin settings should be used.
     *
     * FIXME: Document this
     *.
     * Possible Values:
     * "ALWAYS"            -- the plugin renders at the preferred posion
     *                        no matter what got specified in the DGML file.
     * "SPECIFIED"         -- renders only in case it got specified in the
     *                        DGML file.
     * "SPECIFIED_ALWAYS"  -- In case it got specified in the DGML file
     *                        render according to the specification
     *                        otherwise choose the preferred position
     */
    virtual QString renderPolicy() const = 0;

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
     * "ATMOSPHERE"
     * "ORBIT"
     * "ALWAYS_ON_TOP"
     * "FLOAT_ITEM"
     */
    virtual QStringList renderPosition() const = 0;

    /**
     * @brief Renders the content provided by the plugin on the viewport.
     * @return @c true  Returns whether the rendering has been successful
     */
    virtual bool render( GeoPainter *painter, ViewportParams *viewport,
                         const QString& renderPos = "NONE", GeoSceneLayer * layer = 0 ) = 0;
};

}

Q_DECLARE_INTERFACE( Marble::RenderPluginInterface, "org.kde.Marble.RenderPluginInterface/1.04" )

#endif // MARBLERENDERPLUGININTERFACEH
