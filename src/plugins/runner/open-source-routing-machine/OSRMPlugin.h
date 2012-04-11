//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2012      Dennis Nienhüser <earthwings@gentoo.org>
//


#ifndef MARBLE_OSRMPLUGIN_H
#define MARBLE_OSRMPLUGIN_H

#include "RunnerPlugin.h"

namespace Marble
{

class OSRMPlugin : public RunnerPlugin
{
    Q_OBJECT
    Q_INTERFACES( Marble::RunnerPlugin )

public:
    explicit OSRMPlugin( QObject *parent = 0 );

    QString name() const;

    QString guiString() const;

    QString nameId() const;

    QString version() const;

    QString description() const;

    QString copyrightYears() const;

    QList<PluginAuthor> pluginAuthors() const;

    virtual MarbleAbstractRunner* newRunner() const;

    virtual bool supportsTemplate( RoutingProfilesModel::ProfileTemplate profileTemplate ) const;
};

}

#endif
