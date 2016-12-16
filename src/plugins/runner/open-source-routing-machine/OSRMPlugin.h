//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2012      Dennis Nienhüser <nienhueser@kde.org>
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
    Q_INTERFACES( Marble::RoutingRunnerPlugin )

public:
    explicit OSRMPlugin( QObject *parent = 0 );

    QString name() const override;

    QString guiString() const override;

    QString nameId() const override;

    QString version() const override;

    QString description() const override;

    QString copyrightYears() const override;

    QVector<PluginAuthor> pluginAuthors() const override;

    RoutingRunner *newRunner() const override;

    bool supportsTemplate( RoutingProfilesModel::ProfileTemplate profileTemplate ) const override;
};

}

#endif
