//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2012      Bernhard Beschow <bbeschow@cs.tu-berlin.de>

#ifndef MARBLE_LOGFILE_PLUGIN_H
#define MARBLE_LOGFILE_PLUGIN_H

#include "ParseRunnerPlugin.h"

namespace Marble
{

class LogfilePlugin : public ParseRunnerPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.kde.marble.LogPlugin")
    Q_INTERFACES( Marble::ParseRunnerPlugin )

public:
    explicit LogfilePlugin( QObject *parent = 0 );

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

#endif // MARBLE_LOGFILE_PLUGIN_H
