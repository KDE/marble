//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2008 Torsten Rahn <tackat@kde.org>
// Copyright 2009 Jens-Michael Hoffmann <jensmh@gmx.de>
//


// Own
#include "LayerManager.h"

// Qt
#include <QtGui/QAction>

// Local dir
#include "MarbleDebug.h"
#include "AbstractDataPlugin.h"
#include "AbstractDataPluginItem.h"
#include "AbstractFloatItem.h"
#include "GeoPainter.h"
#include "GeoSceneDocument.h"
#include "GeoSceneSettings.h"
#include "MarbleDataFacade.h"
#include "PluginManager.h"
#include "RenderPlugin.h"
#include "ViewParams.h"
#include "LayerInterface.h"

namespace Marble
{

/**
  * Returns true if the zValue of one is lower than that of two. Null must not be passed
  * as parameter.
  */
bool zValueLessThan( const LayerInterface * const one, const LayerInterface * const two )
{
    Q_ASSERT( one && two );
    return one->zValue() < two->zValue();
}

class LayerManagerPrivate
{
 public:
    LayerManagerPrivate( MarbleDataFacade* dataFacade,
                         PluginManager* pluginManager );
    ~LayerManagerPrivate();

    GeoSceneDocument *m_mapTheme;

    MarbleDataFacade *m_dataFacade;
    PluginManager *m_pluginManager;
    QList<RenderPlugin *> m_renderPlugins;
    QList<AbstractFloatItem *> m_floatItems;
    QList<AbstractDataPlugin *> m_dataPlugins;
    QList<LayerInterface *> m_internalLayers;
};

LayerManagerPrivate::LayerManagerPrivate( MarbleDataFacade* dataFacade,
                                          PluginManager* pluginManager )
    : m_mapTheme( 0 ),
      m_dataFacade( dataFacade ),
      m_pluginManager( pluginManager ),
      m_renderPlugins( pluginManager->createRenderPlugins() )
{
}

LayerManagerPrivate::~LayerManagerPrivate()
{
    foreach( RenderPlugin * renderPlugin, m_renderPlugins )
        renderPlugin->setDataFacade( 0 );
    qDeleteAll( m_renderPlugins );
}


LayerManager::LayerManager( MarbleDataFacade* dataFacade,
                            PluginManager* pluginManager, QObject *parent )
    : QObject( parent ),
      d( new LayerManagerPrivate( dataFacade, pluginManager) )
{

    // get float items and data plugins
    foreach( RenderPlugin * renderPlugin, d->m_renderPlugins ) {
        connect( renderPlugin, SIGNAL( settingsChanged( QString ) ),
                 this, SIGNAL( pluginSettingsChanged() ) );
        connect( renderPlugin, SIGNAL( repaintNeeded( QRegion ) ),
                 this, SIGNAL( repaintNeeded( QRegion ) ) );

        AbstractFloatItem * const floatItem =
            qobject_cast<AbstractFloatItem *>( renderPlugin );
        if ( floatItem )
            d->m_floatItems.append( floatItem );
        
        AbstractDataPlugin * const dataPlugin = 
            qobject_cast<AbstractDataPlugin *>( renderPlugin );
        if( dataPlugin )
            d->m_dataPlugins.append( dataPlugin );
    }

    // Just for initial testing
    foreach( RenderPlugin * renderPlugin, d->m_renderPlugins ) {
        renderPlugin->setDataFacade( d->m_dataFacade );
    }
}

LayerManager::~LayerManager()
{
    delete d;
}

QList<RenderPlugin *> LayerManager::renderPlugins() const
{
    return d->m_renderPlugins;
}

QList<AbstractFloatItem *> LayerManager::floatItems() const
{
    return d->m_floatItems;
}

QList<AbstractDataPlugin *> LayerManager::dataPlugins() const
{
    return d->m_dataPlugins;
}

QList<AbstractDataPluginItem *> LayerManager::whichItemAt( const QPoint& curpos ) const
{
    QList<AbstractDataPluginItem *> itemList;

    foreach( AbstractDataPlugin *plugin, d->m_dataPlugins ) {
        itemList.append( plugin->whichItemAt( curpos ) );
    }
    return itemList;
}

void LayerManager::renderLayers( GeoPainter *painter, ViewParams *viewParams,
                                 const QStringList& renderPositions )
{    
    foreach( const QString& renderPosition, renderPositions ) {
        renderLayer( painter, viewParams, renderPosition );
    }
}

void LayerManager::renderLayer( GeoPainter *painter, ViewParams *viewParams,
                                const QString& renderPosition )
{
    if ( !viewParams || !viewParams->viewport() ) {
        mDebug() << "LayerManager: No valid viewParams set!";
        return;
    }

    ViewportParams* viewport = viewParams->viewport();

    QList<LayerInterface*> layers;

    foreach( RenderPlugin *renderPlugin, d->m_renderPlugins ) {
        if ( renderPlugin && renderPlugin->renderPosition().contains( renderPosition )  ){
            if ( renderPlugin->enabled() && renderPlugin->visible() ) {
                if ( !renderPlugin->isInitialized() )
                {
                    renderPlugin->initialize();
                    emit renderPluginInitialized( renderPlugin );
                }
                layers.push_back( renderPlugin );
            }
        }
    }

    foreach( LayerInterface *layer, d->m_internalLayers ) {
        if ( layer && layer->renderPosition().contains( renderPosition ) ) {
            layers.push_back( layer );
        }
    }

    qSort( layers.begin(), layers.end(), zValueLessThan );
    foreach( LayerInterface *layer, layers ) {
        layer->render( painter, viewport, renderPosition );
    }
}

void LayerManager::loadLayers()
{
}

void LayerManager::syncViewParamsAndPlugins( GeoSceneDocument *mapTheme )
{
    d->m_mapTheme = mapTheme;

    foreach( RenderPlugin * renderPlugin, d->m_renderPlugins ) {
        bool propertyAvailable = false;
        mapTheme->settings()->propertyAvailable( renderPlugin->nameId(), 
						 propertyAvailable );
        bool propertyValue = false;
        mapTheme->settings()->propertyValue( renderPlugin->nameId(), 
					     propertyValue );

        if ( propertyAvailable ) {
            renderPlugin->setVisible( propertyValue );
        }

        disconnect( renderPlugin->action(), SIGNAL( changed() ),
                 this,                   SIGNAL( floatItemsChanged() ) );
        disconnect( renderPlugin, SIGNAL( visibilityChanged( QString, bool ) ),
                 this,         SLOT( syncPropertyWithAction( QString, bool ) ) );
        connect( renderPlugin->action(), SIGNAL( changed() ), 
                 this,                   SIGNAL( floatItemsChanged() ) );
        connect( renderPlugin, SIGNAL( visibilityChanged( QString, bool ) ),
                 this,         SLOT( syncPropertyWithAction( QString, bool ) ) );

    }

    disconnect( mapTheme->settings(), 0, this, 0 );
    connect( mapTheme->settings(), SIGNAL( valueChanged( QString, bool ) ),
             this,                 SLOT( syncActionWithProperty( QString, bool ) ) );
}

void LayerManager::syncActionWithProperty( QString nameId, bool checked )
{
    foreach( RenderPlugin * renderPlugin, d->m_renderPlugins ) {
        if ( nameId == renderPlugin->nameId() ) {
            if ( renderPlugin->visible() == checked )
                return;

            renderPlugin->setVisible( checked );

            return;
        }
    }
}

void LayerManager::syncPropertyWithAction( QString nameId, bool checked )
{
    bool propertyAvailable = false;
    d->m_mapTheme->settings()->propertyAvailable( nameId, propertyAvailable );
 
    if ( propertyAvailable ) {
        bool propertyValue = false;
        d->m_mapTheme->settings()->propertyValue( nameId, propertyValue );

        if ( propertyValue == checked ) {
            return;
        }
        d->m_mapTheme->settings()->setPropertyValue( nameId, checked );
    }
}

void LayerManager::addLayer(LayerInterface *layer)
{
    d->m_internalLayers.push_back(layer);
}

void LayerManager::removeLayer(LayerInterface *layer)
{
    d->m_internalLayers.removeAll(layer);
}

}

#include "LayerManager.moc"
