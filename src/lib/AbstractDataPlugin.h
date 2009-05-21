//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009      Bastian Holst <bastianholst@gmx.de>
//

#ifndef ABSTRACTDATAPLUGIN_H
#define ABSTRACTDATAPLUGIN_H

// Marble
#include "marble_export.h"
#include "RenderPlugin.h"

class QEvent;

namespace Marble {
    
class Geopainter;
class ViewportParams;
class GeoSceneLayer;
class AbstractDataPluginItem;
class AbstractDataPluginModel;
class AbstractDataPluginPrivate;

class MARBLE_EXPORT AbstractDataPlugin : public RenderPlugin {
    Q_OBJECT
    Q_INTERFACES( Marble::RenderPluginInterface )
    
 public:    
    AbstractDataPlugin();

    virtual ~AbstractDataPlugin();    
    /**
     * @brief Returns the name(s) of the backend that the plugin can render
     */
    QStringList backendTypes() const;
    
    /**
     * @brief Return how the plugin settings should be used.
     */
    QString renderPolicy() const;
    
    /**
     * @brief Preferred level in the layer stack for the rendering
     */
    QStringList renderPosition() const;
    
    /**
     * @brief Renders the content provided by the plugin on the viewport.
     * @return @c true  Returns whether the rendering has been successful
     */
    bool render( GeoPainter *painter, ViewportParams *viewport,
                 const QString& renderPos = "NONE", GeoSceneLayer * layer = 0 );

    virtual void initialize() = 0;

    bool isInitialized() const;
    
    AbstractDataPluginModel *model() const;
    
    /**
     * Set the model of the plugin.
     */
    void setModel( AbstractDataPluginModel* model );
    
    void setNameId( const QString& name );
    
    QString nameId() const;
    
    quint32 numberOfItems() const;
    
    void setNumberOfItems( quint32 number );
    
    QList<AbstractDataPluginItem *> whichItemAt( const QPoint& curpos );
    
 private:
    AbstractDataPluginPrivate * const d;
};
    
}

#endif // PANORAMIOMODEL_H
