/*
    This file is part of the Marble Virtual Globe.

    This program is free software licensed under the GNU LGPL. You can
    find a copy of this license in LICENSE.txt in the top directory of
    the source code.

    Copyright 2013 Ander Pijoan <ander.pijoan@deusto.es>
    Copyright 2019 John Zaitseff <J.Zaitseff@zap.org.au>
*/

#ifndef MARBLE_JSONPLUGIN_H
#define MARBLE_JSONPLUGIN_H

#include "ParseRunnerPlugin.h"

namespace Marble
{

class JsonPlugin : public ParseRunnerPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.kde.marble.JsonPlugin")
    Q_INTERFACES( Marble::ParseRunnerPlugin )

public:
    explicit JsonPlugin( QObject *parent = nullptr );

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

#endif
