//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2008 Torsten Rahn <tackat@kde.org>"
//


#ifndef MARBLE_LAYER_INTERFACE_H
#define MARBLE_LAYER_INTERFACE_H

#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QtPlugin>
#include <QtGui/QIcon>

class GeoPainter;
class GeoSceneLayer;
class ViewportParams;

/**
 * @short The class that specifies the Marble layer interface of a plugin.
 *
 */

class MarbleLayerInterface
{
 public:
    virtual ~MarbleLayerInterface(){}

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
     * @brief Preferred position in the layer stack for the rendering
     * Gives a preferred position where in the existing layer stack
     * the render() method of this plugin should get executed.
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
     * @brief Preferred position in the layer stack for the rendering
     * Gives a preferred position where in the existing layer stack
     * the render() method of this plugin should get executed.
     *.
     * Possible Values: 
     * "NONE", "BEHIND_TARGET", "SURFACE", "ATMOSPHERE", "ORBIT", "ALWAYS_ON_TOP", "FLOAT_ITEM"
     */
    virtual QStringList renderPosition() const = 0;

    /**
     * @brief Returns the "real name" of the backend.
     *.
     * Example: "Starry Sky Plugin"
     */
    virtual QString name() const = 0;

    /**
     * @brief Returns the name ID of the backend.
     *.
     * Example: "starrysky"
     */
    virtual QString nameId() const = 0;

    /**
     * @brief Returns a user description of the plugin.
     */
    virtual QString description() const = 0;

    /**
     * @brief Returns an icon for the plugin.
     */
    virtual QIcon icon () const = 0;


    virtual void initialize () = 0;

    virtual bool isInitialized () const = 0;


    /**
     * @brief Renders the content provided by the layer on the viewport.
     * @return @c true  Returns whether the rendering has been successful
     */
    virtual bool render( GeoPainter *painter, ViewportParams *viewport, const QString& renderPos = "NONE", GeoSceneLayer * layer = 0 ) = 0;
};

Q_DECLARE_INTERFACE( MarbleLayerInterface, "org.kde.Marble.MarbleLayerInterface/1.0" )

#endif // MARBLE_LAYER_INTERFACE_H
