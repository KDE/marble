//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011 Thibaut Gridel <tgridel@free.fr>

#ifndef MARBLEOSMPLUGIN_H
#define MARBLEOSMPLUGIN_H

#include "ParseRunnerPlugin.h"

namespace Marble
{

class OsmPlugin : public ParseRunnerPlugin
{
    Q_OBJECT
    Q_INTERFACES( Marble::ParseRunnerPlugin )

public:
    explicit OsmPlugin( QObject *parent = 0 );

    QString name() const;

    QString nameId() const;

    QString version() const;

    QString description() const;

    QString copyrightYears() const;

    QList<PluginAuthor> pluginAuthors() const;

    QString fileFormatDescription() const;

    QStringList fileExtensions() const;

    virtual ParsingRunner* newRunner() const;
};

}
#endif // MARBLEOSMPLUGIN_H
