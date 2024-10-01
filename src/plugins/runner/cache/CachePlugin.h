// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2011 Thibaut Gridel <tgridel@free.fr>

#ifndef MARBLECACHEPLUGIN_H
#define MARBLECACHEPLUGIN_H

#include "ParseRunnerPlugin.h"

namespace Marble
{

class CachePlugin : public ParseRunnerPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.kde.marble.CachePlugin")
    Q_INTERFACES(Marble::ParseRunnerPlugin)

public:
    explicit CachePlugin(QObject *parent = nullptr);

    QString name() const override;

    QString nameId() const override;

    QString version() const override;

    QString description() const override;

    QString copyrightYears() const override;

    QList<PluginAuthor> pluginAuthors() const override;

    QString fileFormatDescription() const override;

    QStringList fileExtensions() const override;

    ParsingRunner *newRunner() const override;
};

}
#endif // MARBLECACHEPLUGIN_H
