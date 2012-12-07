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
        return one->guiString().replace( "&", "" ) < two->guiString().replace( "&", "" );
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
