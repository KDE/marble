//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2008 Torsten Rahn <tackat@kde.org>
// Copyright 2009 Jens-Michael Hoffmann <jensmh@gmx.de>
// Copyright 2011 Bernahrd Beschow <bbeschow@cs.tu-berlin.de>
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
#include "MarbleModel.h"
#include "PluginManager.h"
#include "RenderPlugin.h"
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
    LayerManagerPrivate( const MarbleModel* model );
    ~LayerManagerPrivate();

    GeoSceneDocument *m_mapTheme;

    const MarbleModel *m_marbleModel;
    const PluginManager *m_pluginManager;
    QList<RenderPlugin *> m_renderPlugins;
    QList<AbstractFloatItem *> m_floatItems;
    QList<AbstractDataPlugin *> m_dataPlugins;
    QList<LayerInterface *> m_internalLayers;

    bool m_showBackground;
};

LayerManagerPrivate::LayerManagerPrivate( const MarbleModel* model )
    : m_mapTheme( 0 ),
      m_marbleModel( model ),
      m_pluginManager( model->pluginManager() ),
      m_renderPlugins( m_pluginManager->createRenderPlugins( model ) ),
      m_showBackground( true )
{
}

LayerManagerPrivate::~LayerManagerPrivate()
{
    qDeleteAll( m_renderPlugins );
}


LayerManager::LayerManager( const MarbleModel* model, QObject *parent )
    : QObject( parent ),
      d( new LayerManagerPrivate( model ) )
{

    // get float items and data plugins
    foreach( RenderPlugin * renderPlugin, d->m_renderPlugins ) {
        connect( renderPlugin, SIGNAL( settingsChanged( QString ) ),
                 this, SIGNAL( pluginSettingsChanged() ) );
        connect( renderPlugin, SIGNAL( settingsChanged( QString ) ),
                 this, SIGNAL( repaintNeeded() ) );
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
}

LayerManager::~LayerManager()
{
    delete d;
}

bool LayerManager::showBackground() const
{
    return d->m_showBackground;
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

void LayerManager::renderLayers( GeoPainter *painter, ViewportParams *viewport )
{
    QStringList renderPositions;

    if ( d->m_showBackground ) {
        renderPositions << "STARS" << "BEHIND_TARGET";
    }

    renderPositions << "SURFACE" << "HOVERS_ABOVE_SURFACE" << "ATMOSPHERE"
                    << "ORBIT" << "ALWAYS_ON_TOP" << "FLOAT_ITEM" << "USER_TOOLS";

    foreach( const QString& renderPosition, renderPositions ) {
        renderLayer( painter, viewport, renderPosition );
    }
}

void LayerManager::renderLayer( GeoPainter *painter, ViewportParams *viewport,
                                const QString& renderPosition )
{
    if ( !viewport ) {
        mDebug() << "LayerManager: No valid viewParams set!";
        return;
    }

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
        layer->render( painter, viewport, renderPosition, 0 );
    }
}

void LayerManager::setShowBackground( bool show )
{
    d->m_showBackground = show;
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
                 this,                   SIGNAL( repaintNeeded() ) );
        disconnect( renderPlugin, SIGNAL( visibilityChanged( QString, bool ) ),
                 this,         SLOT( syncPropertyWithAction( QString, bool ) ) );
        connect( renderPlugin->action(), SIGNAL( changed() ), 
                 this,                   SIGNAL( repaintNeeded() ) );
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
