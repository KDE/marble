//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010      Dennis Nienhüser <earthwings@gentoo.org>
//


#ifndef MARBLE_LOCALDATABASEPLUGIN_H
#define MARBLE_LOCALDATABASEPLUGIN_H

#include "SearchRunnerPlugin.h"

namespace Marble
{

class LocalDatabasePlugin : public SearchRunnerPlugin
{
    Q_OBJECT
    Q_INTERFACES( Marble::SearchRunnerPlugin )

public:
    explicit LocalDatabasePlugin(QObject *parent = 0);

    QString name() const;

    QString guiString() const;

    QString nameId() const;

    QString version() const;

    QString description() const;

    QString copyrightYears() const;

    QList<PluginAuthor> pluginAuthors() const;

    virtual MarbleAbstractRunner* newRunner() const;
};

}

#endif
