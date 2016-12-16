//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010      Dennis Nienhüser <nienhueser@kde.org>
//


#ifndef MARBLE_LOCALDATABASEPLUGIN_H
#define MARBLE_LOCALDATABASEPLUGIN_H

#include "SearchRunnerPlugin.h"

namespace Marble
{

class LocalDatabasePlugin : public SearchRunnerPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.kde.marble.LocalDatabasePlugin")
    Q_INTERFACES( Marble::SearchRunnerPlugin )

public:
    explicit LocalDatabasePlugin(QObject *parent = 0);

    QString name() const override;

    QString guiString() const override;

    QString nameId() const override;

    QString version() const override;

    QString description() const override;

    QString copyrightYears() const override;

    QVector<PluginAuthor> pluginAuthors() const override;

    SearchRunner* newRunner() const override;
};

}

#endif
