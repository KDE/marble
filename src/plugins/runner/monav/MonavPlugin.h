// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2010 Dennis Nienhüser <nienhueser@kde.org>
//

#ifndef MARBLE_MONAVPLUGIN_H
#define MARBLE_MONAVPLUGIN_H

#include "RoutingRunnerPlugin.h"

namespace Marble
{

class MonavMapsModel;
class MonavPluginPrivate;
class RouteRequest;

class MonavPlugin : public RoutingRunnerPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.kde.marble.MonavPlugin")
    Q_INTERFACES(Marble::RoutingRunnerPlugin)

public:
    enum MonavRoutingDaemonVersion { Monav_0_2, Monav_0_3 };

    explicit MonavPlugin(QObject *parent = nullptr);

    QString name() const override;

    QString guiString() const override;

    QString nameId() const override;

    QString version() const override;

    QString description() const override;

    QString copyrightYears() const override;

    QList<PluginAuthor> pluginAuthors() const override;

    ~MonavPlugin() override;

    RoutingRunner *newRunner() const override;

    bool supportsTemplate(RoutingProfilesModel::ProfileTemplate profileTemplate) const override;

    QHash<QString, QVariant> templateSettings(RoutingProfilesModel::ProfileTemplate profileTemplate) const override;

    ConfigWidget *configWidget() override;

    bool canWork() const override;

    QString mapDirectoryForRequest(const RouteRequest *request) const;

    QStringList mapDirectoriesForRequest(const RouteRequest *request) const;

    MonavMapsModel *installedMapsModel();

    void reloadMaps();

    MonavRoutingDaemonVersion monavVersion() const;

private:
    MonavPluginPrivate *const d;

    Q_PRIVATE_SLOT(d, void stopDaemon())
};

}

#endif
