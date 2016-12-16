//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010      Dennis Nienhüser <nienhueser@kde.org>
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
    explicit OpenRouteServicePlugin( QObject *parent = 0 );

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
