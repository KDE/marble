//
// This file is part of the Marble Virtual Globe.
//
// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2010 Dennis Nienhüser <nienhueser@kde.org>
//


#ifndef MARBLE_GOSMOREROUTINGPLUGIN_H
#define MARBLE_GOSMOREROUTINGPLUGIN_H

#include "RoutingRunnerPlugin.h"

namespace Marble
{

class GosmorePlugin : public RoutingRunnerPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.kde.marble.GosmoreRoutingPlugin")
    Q_INTERFACES( Marble::RoutingRunnerPlugin )

public:
    explicit GosmorePlugin( QObject *parent = nullptr );

    QString name() const override;

    QString guiString() const override;

    QString nameId() const override;

    QString version() const override;

    QString description() const override;

    QString copyrightYears() const override;

    QVector<PluginAuthor> pluginAuthors() const override;

    RoutingRunner *newRunner() const override;

    bool supportsTemplate(RoutingProfilesModel::ProfileTemplate profileTemplate) const override;

    bool canWork() const override;
};

}

#endif
