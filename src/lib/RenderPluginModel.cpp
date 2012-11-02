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

#include "MarbleWidget.h"
#include "RenderPlugin.h"

namespace Marble
{

class RenderPluginModel::Private
{
public:
    Private( MarbleWidget *marbleWidget );

    static bool renderPluginGuiStringLessThan( RenderPlugin* one, RenderPlugin* two )
    {
        // Sort by gui string ignoring keyboard accelerators
        return one->guiString().replace( "&", "" ) < two->guiString().replace( "&", "" );
    }

    MarbleWidget *const m_marbleWidget;
};

RenderPluginModel::Private::Private( MarbleWidget *marbleWidget ) :
    m_marbleWidget( marbleWidget )
{
}

RenderPluginModel::RenderPluginModel( MarbleWidget *marbleWidget, QObject *parent ) :
    QStandardItemModel( parent ),
    d( new Private( marbleWidget ) )
{
    QList<RenderPlugin *> pluginList = marbleWidget->renderPlugins();
    qSort( pluginList.begin(), pluginList.end(), Private::renderPluginGuiStringLessThan );

    QStandardItem *parentItem = invisibleRootItem();
    foreach ( RenderPlugin *plugin, pluginList ) {
        parentItem->appendRow( plugin->item() );
    }

}

RenderPluginModel::~RenderPluginModel()
{
    // our model doesn't own the items, so take them away
    while ( invisibleRootItem()->hasChildren() ) {
        invisibleRootItem()->takeRow( 0 );
    }

    delete d;
}

void RenderPluginModel::retrievePluginState()
{
    foreach ( RenderPlugin *plugin, d->m_marbleWidget->renderPlugins() ) {
        plugin->retrieveItemState();
    }
}

void RenderPluginModel::applyPluginState()
{
    foreach ( RenderPlugin *plugin, d->m_marbleWidget->renderPlugins() ) {
        plugin->applyItemState();
    }
}

}

#include "RenderPluginModel.moc"
