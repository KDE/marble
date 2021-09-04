//
// This file is part of the Marble Virtual Globe.
//
// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2012 Bernhard Beschow <bbeschow@cs.tu-berlin.de>

#ifndef MARBLE_LOG_PLUGIN_H
#define MARBLE_LOG_PLUGIN_H

#include "ParseRunnerPlugin.h"

namespace Marble
{

class LogfilePlugin : public ParseRunnerPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.kde.marble.LogPlugin")
    Q_INTERFACES( Marble::ParseRunnerPlugin )

public:
    explicit LogfilePlugin( QObject *parent = nullptr );

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
