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

#ifndef MARBLE_RENDERPLUGININTERFACE_H
#define MARBLE_RENDERPLUGININTERFACE_H

#include <QtCore/QtPlugin>

#include "PluginInterface.h"
#include "LayerInterface.h"

namespace Marble
{

class RenderPlugin;

/**
 * @short The class that specifies the Marble layer interface of a plugin.
 *
 */

class RenderPluginInterface: public PluginInterface, public LayerInterface
{
 public:
    virtual ~RenderPluginInterface();
    
    /**
    * @brief Returns a new object of the plugin
    */
    virtual RenderPlugin * newInstance() = 0;

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
     * "ALWAYS"            -- the plugin renders at the preferred position
     *                        no matter what got specified in the DGML file.
     * "SPECIFIED"         -- renders only in case it got specified in the
     *                        DGML file.
     * "SPECIFIED_ALWAYS"  -- In case it got specified in the DGML file
     *                        render according to the specification
     *                        otherwise choose the preferred position
     */
    virtual QString renderPolicy() const = 0;
};

}

Q_DECLARE_INTERFACE( Marble::RenderPluginInterface, "org.kde.Marble.RenderPluginInterface/1.08" )

#endif
