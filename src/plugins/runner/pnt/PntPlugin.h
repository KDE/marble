// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2011 Thibaut Gridel <tgridel@free.fr>

#ifndef MARBLEPNTPLUGIN_H
#define MARBLEPNTPLUGIN_H

#include "ParseRunnerPlugin.h"

namespace Marble
{

class PntPlugin : public ParseRunnerPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.kde.marble.PntPlugin")
    Q_INTERFACES(Marble::ParseRunnerPlugin)

public:
    explicit PntPlugin(QObject *parent = nullptr);

    QString name() const override;

    QString nameId() const override;

    QString version() const override;

    QString description() const override;

    QString copyrightYears() const override;

    QVector<PluginAuthor> pluginAuthors() const override;

    QString fileFormatDescription() const override;

    QStringList fileExtensions() const override;

    ParsingRunner *newRunner() const override;
};

}
#endif // MARBLEPNTPLUGIN_H
