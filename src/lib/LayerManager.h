//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2006-2008 Torsten Rahn <tackat@kde.org>
//

#ifndef MARBLE_LAYERMANAGER_H
#define MARBLE_LAYERMANAGER_H

// Qt
#include <QtCore/QList>
#include <QtCore/QObject>
#include <QtCore/QString>

class QPoint;
class QRegion;

namespace Marble
{

class AbstractDataPlugin;
class AbstractDataPluginItem;
class LayerManagerPrivate;
class GeoPainter;
class GeoSceneDocument;
class ViewportParams;
class RenderPlugin;
class AbstractFloatItem;
class AbstractDataPlugin;
class MarbleModel;
class LayerInterface;

/**
 * @short The class that handles Marble's DGML layers.
 *
 */

class LayerManager : public QObject
{
    Q_OBJECT

 public:
    explicit LayerManager( MarbleModel *model, QObject *parent = 0);
    ~LayerManager();

    void renderLayers( GeoPainter *painter, ViewportParams *viewport, const QStringList& renderPositions );

    void renderLayer( GeoPainter *painter, ViewportParams *viewport, const QString& renderPosition  );

    
    /**
     * @brief Returns a list of all RenderPlugins on the layer, this includes float items
     * @return the list of RenderPlugins
     */
    QList<RenderPlugin *>      renderPlugins() const;
    /**
     * @brief Returns a list of all FloatItems on the layer
     * @return the list of the floatItems
     */
    QList<AbstractFloatItem *> floatItems()    const;
    /**
     * @brief Returns a list of all DataPlugins on the layer
     * @return the list of DataPlugins
     */
    QList<AbstractDataPlugin *> dataPlugins()  const;
    
    /**
     * @brief Returns all items of dataPlugins on the position curpos 
     */
    QList<AbstractDataPluginItem *> whichItemAt( const QPoint& curpos ) const;

    /**
     * @brief Add a layer to be included in rendering.
     */
    void addLayer(LayerInterface *layer);

    /**
     * @brief Remove a layer from being included in rendering.
     */
    void removeLayer(LayerInterface *layer);

 Q_SIGNALS:
    /**
     * @brief Signal that the number of floatItems has changed
     */
    void floatItemsChanged();
    
    /**
     * @brief Signal that a render item has been initialized
     */
    void renderPluginInitialized( RenderPlugin *renderPlugin );

    /**
     * This signal is emitted when the settings of a plugin changed.
     */
    void pluginSettingsChanged();

    /**
     * This signal is emitted when the repaint of the view was requested by a plugin.
     * If available with the @p dirtyRegion which is the region the view will change in.
     * If dirtyRegion.isEmpty() returns true, the whole viewport has to be repainted.
     */
    void repaintNeeded( QRegion dirtyRegion );

 public Q_SLOTS:
    void loadLayers();

    void syncViewParamsAndPlugins( GeoSceneDocument *mapTheme );
    void syncActionWithProperty( QString, bool );
    void syncPropertyWithAction( QString, bool );
 
 private:
    Q_DISABLE_COPY( LayerManager )

    LayerManagerPrivate  * const d;
};

}

#endif
