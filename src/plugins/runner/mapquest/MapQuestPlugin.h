//
// This file is part of the Marble Virtual Globe.
//
// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2012 Dennis Nienhüser <nienhueser@kde.org>
//


#ifndef MARBLE_MAPQUESTPLUGIN_H
#define MARBLE_MAPQUESTPLUGIN_H

#include "RoutingRunnerPlugin.h"

namespace Marble
{

class MapQuestPlugin : public RoutingRunnerPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.kde.marble.MapQuestPlugin")
    Q_INTERFACES( Marble::RoutingRunnerPlugin )

public:
    explicit MapQuestPlugin( QObject *parent = nullptr );

    QString name() const override;

    QString guiString() const override;

    QString nameId() const override;

    QString version() const override;

    QString description() const override;

    QString copyrightYears() const override;

    QVector<PluginAuthor> pluginAuthors() const override;

    RoutingRunner *newRunner() const override;

    ConfigWidget* configWidget() override;

    bool supportsTemplate( RoutingProfilesModel::ProfileTemplate profileTemplate ) const override;
    QHash<QString, QVariant> templateSettings( RoutingProfilesModel::ProfileTemplate profileTemplate ) const override;

};

}

#endif
