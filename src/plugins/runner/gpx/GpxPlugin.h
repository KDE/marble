// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2011 Thibaut Gridel <tgridel@free.fr>

#ifndef MARBLEGPXPLUGIN_H
#define MARBLEGPXPLUGIN_H

#include "ParseRunnerPlugin.h"

namespace Marble
{

class GpxPlugin : public ParseRunnerPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.kde.marble.GpxPlugin")
    Q_INTERFACES( Marble::ParseRunnerPlugin )

public:
    explicit GpxPlugin( QObject *parent = nullptr );

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
#endif // MARBLEGPXPLUGIN_H
