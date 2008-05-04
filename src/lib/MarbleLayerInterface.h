//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2006-2008 Torsten Rahn <tackat@kde.org>"
//


#ifndef MARBLE_LAYER_INTERFACE_H
#define MARBLE_LAYER_INTERFACE_H

#include <QtCore/QString>
#include <QtGui/QIcon>

class ClipPainter;
class GeoSceneLayer;
class ViewportParams;

/**
 * @short The class that handles Marble's DGML layers.
 *
 */

class MarbleLayerInterface
{
    Q_OBJECT

 public:
    virtual ~MarbleLayerInterface();

    /**
     * @brief Returns the name of the backend that the plugin can render
     *.
     * This method should return the name of the backend that the plugin 
     * can render. The string has to be the same one that is given 
     * for the attribute in the layer element of the DGML file that
     * backend is able to process.
     * Examples to replace available default backends would be "vector" or
     * "texture". To provide a completely new kind of functionality please 
     * choose your own unique string.
     */    
    virtual QString backendType() const = 0;

    virtual QString roles() const = 0;


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
    virtual bool render( ClipPainter *painter, GeoSceneLayer * layer, ViewportParams *viewport ) = 0;
};

Q_DECLARE_INTERFACE( MarbleLayerInterface, "org.kde.Marble.MarbleLayerInterface/1.0" )

#endif // MARBLE_LAYER_INTERFACE_H
