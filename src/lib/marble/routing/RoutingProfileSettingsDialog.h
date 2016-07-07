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

#include "RoutingRunnerPlugin.h"

class QStandardItemModel;
class QHBoxLayout;
class Ui_RoutingProfileSettingsDialog;

namespace Marble {

class PluginManager;
class RoutingProfilesModel;

class RoutingProfileSettingsDialog : public QDialog
{
    Q_OBJECT
public:
    RoutingProfileSettingsDialog( const PluginManager *pluginManager, RoutingProfilesModel *profilesModel, QWidget *parent = 0 );
    ~RoutingProfileSettingsDialog();

    void editProfile( int profileIndex );

private Q_SLOTS:
    void updateConfigWidget();

    void openConfigDialog();

private:
    RoutingProfilesModel *m_profilesModel;

    QList<RoutingRunnerPlugin*> m_plugins;
    QHash<RoutingRunnerPlugin*, RoutingRunnerPlugin::ConfigWidget*> m_configWidgets;

    Ui_RoutingProfileSettingsDialog *m_ui;
    QStandardItemModel *m_servicesModel;

    QDialog* m_dialog;
    QHBoxLayout* m_dialogLayout;
};

}

#endif
