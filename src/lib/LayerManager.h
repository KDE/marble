//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2006-2008 Torsten Rahn <tackat@kde.org>"
//


#ifndef LAYERMANAGER_H
#define LAYERMANAGER_H

// Qt
#include <QtCore/QList>
#include <QtCore/QObject>
#include <QtCore/QString>

// Local dir
#include "marble_export.h"

class QPoint;

namespace Marble
{

class AbstractDataPlugin;
class AbstractDataPluginItem;
class LayerManagerPrivate;
class GeoPainter;
class GeoSceneDocument;
class ViewParams;
class RenderPlugin;
class AbstractFloatItem;
class AbstractDataPlugin;
class MarbleDataFacade;

/**
 * @short The class that handles Marble's DGML layers.
 *
 */

class MARBLE_EXPORT LayerManager : public QObject
{
    Q_OBJECT

 public:
    explicit LayerManager( MarbleDataFacade*, QObject *parent = 0);
    ~LayerManager();

    void renderLayers( GeoPainter *painter, ViewParams *viewParams, const QStringList& renderPositions );

    void renderLayer( GeoPainter *painter, ViewParams *viewParams, const QString& renderPosition  );

    
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

 Q_SIGNALS:
    /**
     * @brief Signal that the number of floatItems has changed
     */
    void floatItemsChanged();

    /**
     * This signal is emit when the settings of a plugin changed.
     */
    void pluginSettingsChanged();

 public Q_SLOTS:
    void loadLayers();

    void syncViewParamsAndPlugins( Marble::GeoSceneDocument *mapTheme );
    void syncActionWithProperty( QString, bool );
    void syncPropertyWithAction( QString, bool );
 
 private:
    Q_DISABLE_COPY( LayerManager )

    LayerManagerPrivate  * const d;
};

}

#endif // LAYERMANAGER_H
