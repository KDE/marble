// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2012 Bernhard Beschow <bbeschow@cs.tu-berlin.de>
//

#include "RenderPluginModel.h"

#include "DialogConfigurationInterface.h"
#include "RenderPlugin.h"

namespace Marble
{

class Q_DECL_HIDDEN RenderPluginModel::Private
{
public:
    Private();

    static bool renderPluginGuiStringLessThan(RenderPlugin *one, RenderPlugin *two)
    {
        // Sort by gui string ignoring keyboard accelerators
        return one->guiString().remove(QLatin1Char('&')) < two->guiString().remove(QLatin1Char('&'));
    }

    QList<RenderPlugin *> m_renderPlugins;
};

RenderPluginModel::Private::Private()
    : m_renderPlugins()
{
}

RenderPluginModel::RenderPluginModel(QObject *parent)
    : QStandardItemModel(parent)
    , d(new Private)
{
}

RenderPluginModel::~RenderPluginModel()
{
    // our model doesn't own the items, so take them away
    while (invisibleRootItem()->hasChildren()) {
        invisibleRootItem()->takeRow(0);
    }

    delete d;
}

void RenderPluginModel::setRenderPlugins(const QList<RenderPlugin *> &renderPlugins)
{
    // our model doesn't own the items, so take them away
    while (invisibleRootItem()->hasChildren()) {
        invisibleRootItem()->takeRow(0);
    }

    d->m_renderPlugins = renderPlugins;
    std::sort(d->m_renderPlugins.begin(), d->m_renderPlugins.end(), Private::renderPluginGuiStringLessThan);

    QStandardItem *parentItem = invisibleRootItem();
    for (RenderPlugin *plugin : d->m_renderPlugins) {
        parentItem->appendRow(plugin->item());
    }
}

QList<PluginAuthor> RenderPluginModel::pluginAuthors(const QModelIndex &index) const
{
    if (!index.isValid())
        return {};

    if (index.row() < 0 || index.row() >= d->m_renderPlugins.count())
        return {};

    return d->m_renderPlugins.at(index.row())->pluginAuthors();
}

DialogConfigurationInterface *RenderPluginModel::pluginDialogConfigurationInterface(const QModelIndex &index)
{
    if (!index.isValid())
        return nullptr;

    if (index.row() < 0 || index.row() >= d->m_renderPlugins.count())
        return nullptr;

    RenderPlugin *plugin = d->m_renderPlugins.at(index.row());
    return qobject_cast<DialogConfigurationInterface *>(plugin);
}

void RenderPluginModel::retrievePluginState()
{
    for (RenderPlugin *plugin : d->m_renderPlugins) {
        plugin->retrieveItemState();
    }
}

void RenderPluginModel::applyPluginState()
{
    for (RenderPlugin *plugin : d->m_renderPlugins) {
        plugin->applyItemState();
    }
}

}

#include "moc_RenderPluginModel.cpp"
