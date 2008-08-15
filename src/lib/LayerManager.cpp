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
#include "MarbleRenderPlugin.h"
#include "MarbleAbstractFloatItem.h"
#include "MarbleDataFacade.h"
#include "ViewParams.h"

namespace Marble
{

class LayerManagerPrivate
{
 public:
    LayerManagerPrivate( MarbleDataFacade* dataFacade )
        : m_mapTheme(0),
          m_dataFacade( dataFacade )
    {
    }

    GeoSceneDocument *m_mapTheme;

    MarbleModel      *m_model;
    MarbleDataFacade *m_dataFacade;
    PluginManager    *m_pluginManager;

    QList<MarbleRenderPlugin *> m_renderPlugins;
};


LayerManager::LayerManager( MarbleDataFacade* dataFacade, QObject *parent )
    : QObject( parent ),
      d( new LayerManagerPrivate( dataFacade) )
{
    d->m_pluginManager = new PluginManager( this );

    // Just for initial testing
    d->m_renderPlugins = d->m_pluginManager->renderPlugins();
    foreach( MarbleRenderPlugin * renderPlugin,  d->m_renderPlugins ) {
        renderPlugin->setDataFacade( d->m_dataFacade );
        renderPlugin->initialize();
    }
}

LayerManager::~LayerManager()
{
    delete d->m_pluginManager;
    delete d;
}

QList<MarbleRenderPlugin *> LayerManager::renderPlugins() const
{
    return d->m_renderPlugins;
}

QList<MarbleAbstractFloatItem *> LayerManager::floatItems() const
{
    return d->m_pluginManager->floatItems();
}

void LayerManager::renderLayers( GeoPainter *painter, ViewParams *viewParams )
{
    if ( !viewParams || !viewParams->viewport() ) {
        qDebug() << "LayerManager: No valid viewParams set!";
        return;
    }

    ViewportParams* viewport = viewParams->viewport();

    foreach( MarbleRenderPlugin *renderPlugin,  d->m_renderPlugins ) {
        if ( renderPlugin ){
            if ( renderPlugin->enabled() && renderPlugin->visible() ) {
                renderPlugin->render( painter, viewport, "ALWAYS_ON_TOP" );
            }
        }
    }

    // Looping a second time through is a quick and dirty way to get 
    // the float items displayed on top:
    foreach( MarbleRenderPlugin * renderPlugin,  d->m_renderPlugins ) {
        if ( renderPlugin->renderPosition().contains("FLOAT_ITEM") ) {
            MarbleAbstractFloatItem *floatItem = dynamic_cast<MarbleAbstractFloatItem *>(renderPlugin);
            if ( floatItem
		 && floatItem->enabled() 
		 && renderPlugin->visible() )
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

    foreach( MarbleRenderPlugin * renderPlugin,  d->m_renderPlugins ) {
        bool propertyAvailable = false;
        mapTheme->settings()->propertyAvailable( renderPlugin->nameId(), 
						 propertyAvailable );
        bool propertyValue = false;
        mapTheme->settings()->propertyValue( renderPlugin->nameId(), 
					     propertyValue );

        if ( propertyAvailable ) {
            renderPlugin->setVisible( propertyValue );
        }

        renderPlugin->disconnect();
        connect( renderPlugin->action(), SIGNAL( changed() ), 
                 this,                   SIGNAL( floatItemsChanged() ) );
        connect( renderPlugin, SIGNAL( valueChanged( QString, bool ) ),
                 this,         SLOT( syncPropertyWithAction( QString, bool ) ) );
    }

    disconnect( mapTheme->settings(), 0, this, 0 );
    connect( mapTheme->settings(), SIGNAL( valueChanged( QString, bool ) ),
             this,                 SLOT( syncActionWithProperty( QString, bool ) ) );
}

void LayerManager::syncActionWithProperty( QString nameId, bool checked )
{
    foreach( MarbleRenderPlugin * renderPlugin,  d->m_renderPlugins ) {
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

}

#include "LayerManager.moc"
