//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011 Thibaut Gridel <tgridel@free.fr>

#ifndef MARBLESHPPLUGIN_H
#define MARBLESHPPLUGIN_H

#include "ParseRunnerPlugin.h"

namespace Marble
{

class ShpPlugin : public ParseRunnerPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.kde.marble.ShpPlugin")
    Q_INTERFACES( Marble::ParseRunnerPlugin )

public:
    explicit ShpPlugin( QObject *parent = 0 );

    QString name() const override;

    QString nameId() const override;

    QString version() const override;

    QString description() const override;

    QString copyrightYears() const override;

    QVector<PluginAuthor> pluginAuthors() const override;

    QString fileFormatDescription() const override;

    QStringList fileExtensions() const override;

    ParsingRunner* newRunner() const override;
};

}
#endif // MARBLESHPPLUGIN_H
