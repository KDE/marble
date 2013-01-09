//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2012      Bernhard Beschow <bbeschow@cs.tu-berlin.de>
//

#include "RenderPluginModel.h"

#include "DialogConfigurationInterface.h"
#include "RenderPlugin.h"

namespace Marble
{

class RenderPluginModel::Private
{
public:
    Private();

    static bool renderPluginGuiStringLessThan( RenderPlugin* one, RenderPlugin* two )
    {
        // Sort by gui string ignoring keyboard accelerators
        return one->guiString().remove( QLatin1Char( '&' ) ) < two->guiString().remove( QLatin1Char( '&' ) );
    }

    QList<RenderPlugin *> m_renderPlugins;
};

RenderPluginModel::Private::Private() :
    m_renderPlugins()
{
}

RenderPluginModel::RenderPluginModel( QObject *parent ) :
    QStandardItemModel( parent ),
    d( new Private )
{
}

RenderPluginModel::~RenderPluginModel()
{
    // our model doesn't own the items, so take them away
    while ( invisibleRootItem()->hasChildren() ) {
        invisibleRootItem()->takeRow( 0 );
    }

    delete d;
}

void RenderPluginModel::setRenderPlugins( const QList<RenderPlugin *> &renderPlugins )
{
    // our model doesn't own the items, so take them away
    while ( invisibleRootItem()->hasChildren() ) {
        invisibleRootItem()->takeRow( 0 );
    }

    d->m_renderPlugins = renderPlugins;
    qSort( d->m_renderPlugins.begin(), d->m_renderPlugins.end(), Private::renderPluginGuiStringLessThan );

    QStandardItem *parentItem = invisibleRootItem();
    foreach ( RenderPlugin *plugin, d->m_renderPlugins ) {
        parentItem->appendRow( plugin->item() );
    }
}

QList<PluginAuthor> RenderPluginModel::pluginAuthors( const QModelIndex &index ) const
{
    if ( !index.isValid() )
        return QList<PluginAuthor>();

    if ( index.row() < 0 || index.row() >= d->m_renderPlugins.count() )
        return QList<PluginAuthor>();

    return d->m_renderPlugins.at( index.row() )->pluginAuthors();
}

DialogConfigurationInterface *RenderPluginModel::pluginDialogConfigurationInterface( const QModelIndex &index )
{
    if ( !index.isValid() )
        return 0;

    if ( index.row() < 0 || index.row() >= d->m_renderPlugins.count() )
        return 0;

    RenderPlugin *plugin = d->m_renderPlugins.at( index.row() );
    return qobject_cast<DialogConfigurationInterface *>( plugin );
}

void RenderPluginModel::retrievePluginState()
{
    foreach ( RenderPlugin *plugin, d->m_renderPlugins ) {
        plugin->retrieveItemState();
    }
}

void RenderPluginModel::applyPluginState()
{
    foreach ( RenderPlugin *plugin, d->m_renderPlugins ) {
        plugin->applyItemState();
    }
}

}

#include "RenderPluginModel.moc"
