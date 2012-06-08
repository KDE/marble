//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2008 Torsten Rahn <tackat@kde.org>
// Copyright 2009 Jens-Michael Hoffmann <jensmh@gmx.de>
// Copyright 2011,2012 Bernahrd Beschow <bbeschow@cs.tu-berlin.de>
//


// Own
#include "LayerManager.h"

// Local dir
#include "MarbleDebug.h"
#include "AbstractDataPlugin.h"
#include "AbstractDataPluginItem.h"
#include "AbstractFloatItem.h"
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

class LayerManager::Private
{
 public:
    Private( const MarbleModel* model, LayerManager *parent );
    ~Private();

    void updateVisibility( bool visible, const QString &nameId );

    void renderLayer( GeoPainter *painter, ViewportParams *viewport, const QString& renderPosition  );

    void addPlugins();

    LayerManager *const q;

    QList<RenderPlugin *> m_renderPlugins;
    QList<AbstractFloatItem *> m_floatItems;
    QList<AbstractDataPlugin *> m_dataPlugins;
    QList<LayerInterface *> m_internalLayers;
    const MarbleModel* m_model;

    bool m_showBackground;
};

LayerManager::Private::Private( const MarbleModel* model, LayerManager *parent )
    : q( parent ),
      m_renderPlugins(),
      m_model( model ),
      m_showBackground( true )
{
}

LayerManager::Private::~Private()
{
    qDeleteAll( m_renderPlugins );
}

void LayerManager::Private::updateVisibility( bool visible, const QString &nameId )
{
    emit q->visibilityChanged( nameId, visible );
}


LayerManager::LayerManager( const MarbleModel* model, QObject *parent )
    : QObject( parent ),
      d( new Private( model, this ) )
{
    d->addPlugins();
    connect( model->pluginManager(), SIGNAL( renderPluginsChanged() ), this, SLOT( addPlugins() ) );
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
        d->renderLayer( painter, viewport, renderPosition );
    }
}

void LayerManager::Private::renderLayer( GeoPainter *painter, ViewportParams *viewport,
                                const QString& renderPosition )
{
    QList<LayerInterface*> layers;

    foreach( RenderPlugin *renderPlugin, m_renderPlugins ) {
        if ( renderPlugin && renderPlugin->renderPosition().contains( renderPosition )  ){
            if ( renderPlugin->enabled() && renderPlugin->visible() ) {
                if ( !renderPlugin->isInitialized() )
                {
                    renderPlugin->initialize();
                    emit q->renderPluginInitialized( renderPlugin );
                }
                layers.push_back( renderPlugin );
            }
        }
    }

    foreach( LayerInterface *layer, m_internalLayers ) {
        if ( layer && layer->renderPosition().contains( renderPosition ) ) {
            layers.push_back( layer );
        }
    }

    qSort( layers.begin(), layers.end(), zValueLessThan );
    foreach( LayerInterface *layer, layers ) {
        layer->render( painter, viewport, renderPosition, 0 );
    }
}

void LayerManager::Private::addPlugins()
{
    foreach ( const RenderPlugin *factory, m_model->pluginManager()->renderPlugins() ) {
        bool alreadyCreated = false;
        foreach( const RenderPlugin* existing, m_renderPlugins ) {
            if ( existing->nameId() == factory->nameId() ) {
                alreadyCreated = true;
                break;
            }
        }

        if ( alreadyCreated ) {
            continue;
        }

        RenderPlugin *const renderPlugin = factory->newInstance( m_model );
        Q_ASSERT( renderPlugin && "Plugin returned null when requesting a new instance." );
        m_renderPlugins.append( renderPlugin );

        QObject::connect( renderPlugin, SIGNAL( settingsChanged( QString ) ),
                 q, SIGNAL( pluginSettingsChanged() ) );
        QObject::connect( renderPlugin, SIGNAL( repaintNeeded( QRegion ) ),
                 q, SIGNAL( repaintNeeded( QRegion ) ) );
        QObject::connect( renderPlugin, SIGNAL( visibilityChanged( bool, const QString & ) ),
                 q, SLOT( updateVisibility( bool, const QString & ) ) );

        // get float items ...
        AbstractFloatItem * const floatItem =
            qobject_cast<AbstractFloatItem *>( renderPlugin );
        if ( floatItem )
            m_floatItems.append( floatItem );

        // ... and data plugins
        AbstractDataPlugin * const dataPlugin =
            qobject_cast<AbstractDataPlugin *>( renderPlugin );
        if( dataPlugin )
            m_dataPlugins.append( dataPlugin );
    }
}

void LayerManager::setShowBackground( bool show )
{
    d->m_showBackground = show;
}

void LayerManager::setVisible( const QString &nameId, bool visible )
{
    foreach( RenderPlugin * renderPlugin, d->m_renderPlugins ) {
        if ( nameId == renderPlugin->nameId() ) {
            if ( renderPlugin->visible() == visible )
                return;

            renderPlugin->setVisible( visible );

            return;
        }
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
