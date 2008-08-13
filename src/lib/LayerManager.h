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
#include <QtCore/QObject>

// Local dir
#include "PluginManager.h"

namespace Marble
{

class LayerManagerPrivate;
class GeoPainter;
class GeoSceneDocument;
class ViewParams;
class MarbleAbstractLayer;
class MarbleAbstractFloatItem;
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

    void renderLayers( GeoPainter *painter, ViewParams *viewParams );

    QList<MarbleAbstractLayer *> layerPlugins() const;
    QList<MarbleAbstractFloatItem *> floatItems() const;

 Q_SIGNALS:
    /**
     * @brief Signal that the number of floatItems has changed
     */
    void floatItemsChanged();

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
