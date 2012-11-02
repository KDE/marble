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

namespace Marble
{

class MarbleWidget;

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
        NameId = Qt::UserRole + 2,       // a QString
        ConfigurationDialogAvailable,    // a bool
        BackendTypes                     // a QStringList
    };

    explicit RenderPluginModel( MarbleWidget *marbleWidget, QObject *parent = 0 );

    ~RenderPluginModel();

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
