//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010      Dennis Nienhüser <earthwings@gentoo.org>
//


#ifndef MARBLE_GOSMOREROUTINGPLUGIN_H
#define MARBLE_GOSMOREROUTINGPLUGIN_H

#include "RoutingRunnerPlugin.h"

namespace Marble
{

class GosmorePlugin : public RoutingRunnerPlugin
{
    Q_OBJECT
    Q_INTERFACES( Marble::RoutingRunnerPlugin )

public:
    explicit GosmorePlugin( QObject *parent = 0 );

    QString name() const;

    QString guiString() const;

    QString nameId() const;

    QString version() const;

    QString description() const;

    QString copyrightYears() const;

    QList<PluginAuthor> pluginAuthors() const;

    virtual RoutingRunner *newRunner() const;

    virtual bool supportsTemplate(RoutingProfilesModel::ProfileTemplate profileTemplate) const;

    virtual bool canWork() const;
};

}

#endif
