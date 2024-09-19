// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2013 Mihail Ivchenko <ematirov@gmail.com>
//

#ifndef MARBLE_CYCLESTREETSPLUGIN_H
#define MARBLE_CYCLESTREETSPLUGIN_H

#include "RoutingRunnerPlugin.h"

namespace Marble
{

class CycleStreetsPlugin : public RoutingRunnerPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.kde.marble.CycleStreetsPlugin")
    Q_INTERFACES(Marble::RoutingRunnerPlugin)

public:
    explicit CycleStreetsPlugin(QObject *parent = nullptr);

    QString name() const override;

    QString guiString() const override;

    QString nameId() const override;

    QString version() const override;

    QString description() const override;

    QString copyrightYears() const override;

    QVector<PluginAuthor> pluginAuthors() const override;

    RoutingRunner *newRunner() const override;

    ConfigWidget *configWidget() override;

    bool supportsTemplate(RoutingProfilesModel::ProfileTemplate profileTemplate) const override;
};

}

#endif
