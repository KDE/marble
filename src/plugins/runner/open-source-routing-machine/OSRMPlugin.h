// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2012 Dennis Nienhüser <nienhueser@kde.org>
//

#ifndef MARBLE_OSRMPLUGIN_H
#define MARBLE_OSRMPLUGIN_H

#include "RoutingRunnerPlugin.h"

namespace Marble
{

class OSRMPlugin : public RoutingRunnerPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.kde.marble.OSRMPlugin")
    Q_INTERFACES(Marble::RoutingRunnerPlugin)

public:
    explicit OSRMPlugin(QObject *parent = nullptr);

    QString name() const override;

    QString guiString() const override;

    QString nameId() const override;

    QString version() const override;

    QString description() const override;

    QString copyrightYears() const override;

    QList<PluginAuthor> pluginAuthors() const override;

    RoutingRunner *newRunner() const override;

    bool supportsTemplate(RoutingProfilesModel::ProfileTemplate profileTemplate) const override;
};

}

#endif
