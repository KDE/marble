//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010      Niko Sams <niko.sams@gmail.com>
//


#ifndef MARBLE_ROUTINGPROFILESETTINGSDIALOG_H
#define MARBLE_ROUTINGPROFILESETTINGSDIALOG_H

#include <QDialog>

#include "RunnerPlugin.h"

class QModelIndex;
class QStandardItemModel;
class Ui_RoutingProfileSettingsDialog;

namespace Marble {

class PluginManager;
class RoutingProfilesModel;
class RunnerPlugin;

class RoutingProfileSettingsDialog : public QDialog
{
    Q_OBJECT
public:
    RoutingProfileSettingsDialog( PluginManager *pluginManager, RoutingProfilesModel *profilesModel, QWidget *parent = 0 );
    ~RoutingProfileSettingsDialog();

    void editProfile( int profileIndex );

private slots:
    void updateConfigWidget();

private:
    PluginManager *m_pluginManager;
    RoutingProfilesModel *m_profilesModel;

    QList<RunnerPlugin*> m_plugins;
    QHash<RunnerPlugin*, RunnerPlugin::ConfigWidget*> m_configWidgets;

    Ui_RoutingProfileSettingsDialog *m_ui;
    QStandardItemModel *m_servicesModel;
};

}

#endif
