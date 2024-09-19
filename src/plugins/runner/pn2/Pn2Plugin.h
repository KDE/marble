// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2012 Cezar Mocan <mocancezar@gmail.com>

#ifndef MARBLEPN2PLUGIN_H
#define MARBLEPN2PLUGIN_H

#include "ParseRunnerPlugin.h"

namespace Marble
{

class Pn2Plugin : public ParseRunnerPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.kde.marble.Pn2Plugin")
    Q_INTERFACES(Marble::ParseRunnerPlugin)

public:
    explicit Pn2Plugin(QObject *parent = nullptr);

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
#endif // MARBLEPN2PLUGIN_H
