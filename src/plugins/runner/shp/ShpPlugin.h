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
    Q_INTERFACES( Marble::ParseRunnerPlugin )

public:
    explicit ShpPlugin( QObject *parent = 0 );

    QString name() const;

    QString nameId() const;

    QString version() const;

    QString description() const;

    QString copyrightYears() const;

    QList<PluginAuthor> pluginAuthors() const;

    QString fileFormatDescription() const;

    QStringList fileExtensions() const;

    virtual MarbleAbstractRunner* newRunner() const;
};

}
#endif // MARBLESHPPLUGIN_H
