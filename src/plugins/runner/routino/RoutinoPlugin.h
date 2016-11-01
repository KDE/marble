//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010      Dennis Nienhüser <nienhueser@kde.org>
//


#ifndef MARBLE_ROUTINOPLUGIN_H
#define MARBLE_ROUTINOPLUGIN_H

#include "RoutingRunnerPlugin.h"

namespace Marble
{

class RoutinoPlugin : public RoutingRunnerPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.kde.marble.RoutinoPlugin")
    Q_INTERFACES( Marble::RoutingRunnerPlugin )

public:
    explicit RoutinoPlugin( QObject *parent = 0 );

    QString name() const;

    QString guiString() const;

    QString nameId() const;

    QString version() const;

    QString description() const;

    QString copyrightYears() const;

    QVector<PluginAuthor> pluginAuthors() const override;

    virtual RoutingRunner *newRunner() const;

    ConfigWidget* configWidget();

    bool supportsTemplate(RoutingProfilesModel::ProfileTemplate profileTemplate) const;

    QHash< QString, QVariant > templateSettings(RoutingProfilesModel::ProfileTemplate profileTemplate) const;

    virtual bool canWork() const;
};

}

#endif
