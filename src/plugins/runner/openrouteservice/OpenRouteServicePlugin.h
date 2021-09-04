//
// This file is part of the Marble Virtual Globe.
//
// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2010 Dennis Nienhüser <nienhueser@kde.org>
//


#ifndef MARBLE_OPENROUTESERVICEPLUGIN_H
#define MARBLE_OPENROUTESERVICEPLUGIN_H

#include "RoutingRunnerPlugin.h"

namespace Marble
{

class OpenRouteServicePlugin : public RoutingRunnerPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.kde.marble.OpenRouteServicePlugin")
    Q_INTERFACES( Marble::RoutingRunnerPlugin )

public:
    explicit OpenRouteServicePlugin( QObject *parent = nullptr );

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
