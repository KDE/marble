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
#include <QtCore/QDebug>
#include <QtGui/QAction>

// Local dir
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

    MarbleDataFacade *m_dataFacade;
    QList<RenderPlugin *> m_renderPlugins;
    QList<AbstractFloatItem *> m_floatItems;
    QList<AbstractDataPlugin *> m_dataPlugins;
};


LayerManager::LayerManager( MarbleDataFacade* dataFacade, QObject *parent )
    : QObject( parent ),
      d( new LayerManagerPrivate( dataFacade) )
{
    PluginManager pm;
    d->m_renderPlugins = pm.createRenderPlugins();

    // get float items and data plugins
    foreach( RenderPlugin * renderPlugin, d->m_renderPlugins ) {
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
        renderPlugin->initialize();
    }
}

LayerManager::~LayerManager()
{
    foreach( RenderPlugin * renderPlugin, d->m_renderPlugins )
        renderPlugin->setDataFacade( 0 );
    qDeleteAll( d->m_renderPlugins );
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

QList<AbstractDataPluginItem *> LayerManager::whichItemAt( const QPoint& curpos ) const {
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
        qDebug() << "LayerManager: No valid viewParams set!";
        return;
    }

    ViewportParams* viewport = viewParams->viewport();

    foreach( RenderPlugin *renderPlugin, d->m_renderPlugins ) {
        if ( renderPlugin && renderPlugin->renderPosition().contains( renderPosition )  ){
            if ( renderPlugin->enabled() && renderPlugin->visible() ) {
                renderPlugin->render( painter, viewport, renderPosition );
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

}

#include "LayerManager.moc"
