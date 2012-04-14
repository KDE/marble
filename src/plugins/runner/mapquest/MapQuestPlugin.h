//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2012      Dennis Nienhüser <earthwings@gentoo.org>
//


#ifndef MARBLE_MAPQUESTPLUGIN_H
#define MARBLE_MAPQUESTPLUGIN_H

#include "RoutingRunnerPlugin.h"

namespace Marble
{

class MapQuestPlugin : public RoutingRunnerPlugin
{
    Q_OBJECT
    Q_INTERFACES( Marble::RoutingRunnerPlugin )

public:
    explicit MapQuestPlugin( QObject *parent = 0 );

    QString name() const;

    QString guiString() const;

    QString nameId() const;

    QString version() const;

    QString description() const;

    QString copyrightYears() const;

    QList<PluginAuthor> pluginAuthors() const;

    virtual MarbleAbstractRunner* newRunner() const;

    ConfigWidget* configWidget();

    virtual bool supportsTemplate( RoutingProfilesModel::ProfileTemplate profileTemplate ) const;
    virtual QHash<QString, QVariant> templateSettings( RoutingProfilesModel::ProfileTemplate profileTemplate ) const;

};

}

#endif
