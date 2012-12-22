//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009      Bastian Holst <bastianholst@gmx.de>
//

#ifndef MARBLE_ABSTRACTDATAPLUGIN_H
#define MARBLE_ABSTRACTDATAPLUGIN_H

// Marble
#include "marble_export.h"
#include "RenderPlugin.h"

class QDeclarativeComponent;
class QGraphicsItem;

namespace Marble
{
    
class ViewportParams;
class GeoSceneLayer;
class AbstractDataPluginItem;
class AbstractDataPluginModel;
class AbstractDataPluginPrivate;
class PluginManager;

/**
 * @short An abstract class for plugins that show data that has a geo coordinate
 *
 * This is the abstract class for plugins that show data on Marble map.
 * It takes care of painting all items it gets from the corresponding AbstractDataPluginModel
 * that has to be set on initialisation.
 *
 * The user has to set the nameId as well as the number of items to fetch.
 * Additionally it should be useful to set standard values via setEnabled (often true)
 * and setVisible (often false) in the constructor of a subclass.
 **/
class MARBLE_EXPORT AbstractDataPlugin : public RenderPlugin
{
    Q_OBJECT

    Q_PROPERTY( bool favoriteItemsOnly READ isFavoriteItemsOnly WRITE setFavoriteItemsOnly NOTIFY favoriteItemsOnlyChanged )
    /** @todo FIXME Qt Quick segfaults if using the real class here instead of QObject */
    Q_PROPERTY( QObject* favoritesModel READ favoritesModel NOTIFY favoritesModelChanged )
    
 public:    
    AbstractDataPlugin( const MarbleModel *marbleModel );

    virtual ~AbstractDataPlugin();

    bool isInitialized() const;

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

    /**
     * @return The model associated with the plugin.
     */
    AbstractDataPluginModel *model() const;
    
    /**
     * Set the model of the plugin.
     */
    void setModel( AbstractDataPluginModel* model );
    
    const PluginManager *pluginManager() const;

    /**
     * Set the number of items to be shown at the same time.
     */
    void setNumberOfItems( quint32 number );
    
    /**
     * @return The number of items to be shown at the same time.
     */
    quint32 numberOfItems() const;
    
    /**
     * This function returns all items at the position @p curpos. Depending on where they have
     * been painted the last time.
     *
     * @return The items at the given position.
     */
    QList<AbstractDataPluginItem *> whichItemAt( const QPoint& curpos );

    /**
     * Function for returning the type of plugin this is for.
     * This affects where in the menu tree the action() is placed.
     *
     * @return: The type of render plugin this is.
     */
    virtual RenderType renderType() const;

    void setDelegate( QDeclarativeComponent* delegate, QGraphicsItem* parent );

    /** Convenience method to set the favorite item state on the current model */
    void setFavoriteItemsOnly( bool favoriteOnly );

    bool isFavoriteItemsOnly() const;

    QObject* favoritesModel();
    
public Q_SLOTS:
    void handleViewportChange( ViewportParams* viewport );

 private Q_SLOTS:
    virtual void favoriteItemsChanged( const QStringList& favoriteItems );

    void delayedUpdate();

 Q_SIGNALS:
    void changedNumberOfItems( quint32 number );

    void favoriteItemsOnlyChanged();

    void favoritesModelChanged();
    
 private:
    AbstractDataPluginPrivate * const d;
};
    
}

#endif
