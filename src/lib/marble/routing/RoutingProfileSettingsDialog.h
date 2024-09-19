// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2010 Niko Sams <niko.sams@gmail.com>
//

#ifndef MARBLE_ROUTINGPROFILESETTINGSDIALOG_H
#define MARBLE_ROUTINGPROFILESETTINGSDIALOG_H

#include <QDialog>

#include "RoutingRunnerPlugin.h"

class QStandardItemModel;
class QHBoxLayout;
class Ui_RoutingProfileSettingsDialog;

namespace Marble
{

class PluginManager;
class RoutingProfilesModel;

class RoutingProfileSettingsDialog : public QDialog
{
    Q_OBJECT
public:
    RoutingProfileSettingsDialog(const PluginManager *pluginManager, RoutingProfilesModel *profilesModel, QWidget *parent = nullptr);
    ~RoutingProfileSettingsDialog() override;

    void editProfile(int profileIndex);

private Q_SLOTS:
    void updateConfigWidget();

    void openConfigDialog();

private:
    RoutingProfilesModel *m_profilesModel;

    QList<RoutingRunnerPlugin *> m_plugins;
    QHash<RoutingRunnerPlugin *, RoutingRunnerPlugin::ConfigWidget *> m_configWidgets;

    Ui_RoutingProfileSettingsDialog *m_ui;
    QStandardItemModel *m_servicesModel;

    QDialog *m_dialog;
    QHBoxLayout *m_dialogLayout;
};

}

#endif
