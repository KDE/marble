//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2012      Bernhard Beschow <bbeschow@cs.tu-berlin.de>
//

#ifndef MARBLE_RENDERPLUGINMODEL_H
#define MARBLE_RENDERPLUGINMODEL_H

#include "marble_export.h"
#include <QtGui/QStandardItemModel>

#include <QtCore/QList>

#include "PluginInterface.h"

namespace Marble
{

class DialogConfigurationInterface;
class RenderPlugin;

/**
 * @brief Provides common access to various kinds of plugins without having to know about their details.
 */
class MARBLE_EXPORT RenderPluginModel : public QStandardItemModel
{
    Q_OBJECT

public:
    /**
     * This enum contains the data roles for the QStandardItems.
     */
    enum ItemDataRole {
        Name = Qt::DisplayRole,          // QString
        Icon = Qt::DecorationRole,       // QIcon
        Description = Qt::ToolTipRole,   // QString
        NameId = Qt::UserRole + 2,       // QString
        ConfigurationDialogAvailable,    // bool
        BackendTypes,                    // QStringList
        Version,                         // QString
        AboutDataText,                   // QString
        CopyrightYears                   // QString
    };

    explicit RenderPluginModel( QObject *parent = 0 );

    ~RenderPluginModel();

    /**
     * @brief Set the RenderPlugins the model should manage.
     *
     * The given plugins must not be deleted as long as the model has a hold on them,
     * i.e. until the model is deleted or a different set of plugins is assigned.
     *
     * @param renderPlugins the RenderPlugins to be managed
     */
    void setRenderPlugins( const QList<RenderPlugin *> &renderPlugins );

    QList<PluginAuthor> pluginAuthors( const QModelIndex &index ) const;

    DialogConfigurationInterface *pluginDialogConfigurationInterface( const QModelIndex &index );

public Q_SLOTS:
    /**
     * Retrieve the current plugin state for the user interface.
     */
    void retrievePluginState();

    /**
     * Apply the plugin state from the user interface.
     */
    void applyPluginState();

private:
    class Private;
    Private *const d;
    friend class Private;
};

}

#endif // MARBLE_RENDERPLUGINMODEL_H
