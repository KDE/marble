//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2008 Torsten Rahn <tackat@kde.org>"
//


// Own
#include "LayerManager.h"

// Qt
#include <QtCore/QDebug>
#include <QtGui/QAction>

// Local dir
#include "GeoPainter.h"
#include "GeoSceneDocument.h"
#include "GeoSceneSettings.h"
#include "MarbleLayerInterface.h"
#include "MarbleAbstractLayer.h"
#include "MarbleAbstractFloatItem.h"
#include "MarbleDataFacade.h"
#include "MarbleModel.h"
#include "ViewParams.h"


class LayerManagerPrivate
{
 public:
    LayerManagerPrivate( MarbleModel* model )
        : m_mapTheme(0),
          m_model( model )
    {
    }

    GeoSceneDocument *m_mapTheme;

    MarbleModel* m_model;
    MarbleDataFacade *m_dataFacade;
    PluginManager *m_pluginManager;

    QList<MarbleAbstractLayer *> m_layerPlugins;
};

LayerManager::LayerManager( MarbleModel* model, QObject *parent )
    : QObject( parent ),
      d( new LayerManagerPrivate( model ) )
{
    d->m_dataFacade = new MarbleDataFacade( model );
    d->m_pluginManager = new PluginManager();

    // Just for initial testing
    d->m_layerPlugins = d->m_pluginManager->layerPlugins();
    foreach( MarbleAbstractLayer * layerPlugin,  d->m_layerPlugins ) {
        layerPlugin->setDataFacade( d->m_dataFacade );
        layerPlugin->initialize();
    }
}

LayerManager::~LayerManager()
{
    delete d->m_pluginManager;
    delete d->m_dataFacade;
    delete d;
}

QList<MarbleAbstractLayer *> LayerManager::layerPlugins() const
{
    return d->m_layerPlugins;
}

QList<MarbleAbstractFloatItem *> LayerManager::floatItems() const
{
    return d->m_pluginManager->floatItems();
}

void LayerManager::renderLayers( GeoPainter *painter, ViewParams *viewParams )
{
    if ( !viewParams || !viewParams->viewport() )
    {
        qDebug() << "LayerManager: No valid viewParams set!";
        return;
    }

    ViewportParams* viewport = viewParams->viewport();

    foreach( MarbleAbstractLayer *layerPlugin,  d->m_layerPlugins ) {
        if ( layerPlugin && layerPlugin->enabled() && layerPlugin->visible() )
        {
            layerPlugin->render( painter, viewport, "ALWAYS_ON_TOP" );
        }
    }

    // Looping a second time through is a quick and dirty way to get 
    // the float items displayed on top:
    foreach( MarbleAbstractLayer * layerPlugin,  d->m_layerPlugins ) {
        if ( layerPlugin->renderPosition().contains("FLOAT_ITEM") )
        {
            MarbleAbstractFloatItem *floatItem = dynamic_cast<MarbleAbstractFloatItem *>(layerPlugin);

            if ( floatItem && floatItem->enabled() && layerPlugin->visible() )
            {
                floatItem->render( painter, viewport, "FLOAT_ITEM" );
            }
        }
    }
}

void LayerManager::loadLayers()
{
}

void LayerManager::syncViewParamsAndPlugins( GeoSceneDocument *mapTheme )
{
    d->m_mapTheme = mapTheme;

    foreach( MarbleAbstractLayer * layerPlugin,  d->m_layerPlugins ) {
        bool propertyAvailable = false;
        mapTheme->settings()->propertyAvailable( layerPlugin->nameId(), propertyAvailable );
        bool propertyValue = false;
        mapTheme->settings()->propertyValue( layerPlugin->nameId(), propertyValue );

        if ( propertyAvailable == true )
        {
            layerPlugin->setVisible( propertyValue );
        }

        layerPlugin->disconnect();
        connect( layerPlugin->action(), SIGNAL( changed() ), 
                 this,                  SIGNAL( floatItemsChanged() ) );
        connect( layerPlugin, SIGNAL( valueChanged( QString, bool ) ),
                 this, SLOT( syncPropertyWithAction( QString, bool ) ) );
    }

    disconnect( mapTheme->settings(), 0, this, 0 );
    connect( mapTheme->settings(), SIGNAL( valueChanged( QString, bool ) ),
             this, SLOT( syncActionWithProperty( QString, bool ) ) );
}

void LayerManager::syncActionWithProperty( QString nameId, bool checked )
{
    foreach( MarbleAbstractLayer * layerPlugin,  d->m_layerPlugins ) {
        if ( nameId == layerPlugin->nameId() ) {
            if ( layerPlugin->visible() == checked )
            {
                return;
            }

            layerPlugin->setVisible( checked );

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

#include "LayerManager.moc"
