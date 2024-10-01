// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2010 Dennis Nienhüser <nienhueser@kde.org>
// SPDX-FileCopyrightText: 2012 Bernhard Beschow <bbeschow@cs.tu-berlin.de>
//

#ifndef MARBLE_GOSMOREREVERSEGEOCODINGPLUGIN_H
#define MARBLE_GOSMOREREVERSEGEOCODINGPLUGIN_H

#include "ReverseGeocodingRunnerPlugin.h"

namespace Marble
{

class GosmorePlugin : public ReverseGeocodingRunnerPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.kde.marble.GosmoreReverseGeocodingPlugin")
    Q_INTERFACES(Marble::ReverseGeocodingRunnerPlugin)

public:
    explicit GosmorePlugin(QObject *parent = nullptr);

    QString name() const override;

    QString guiString() const override;

    QString nameId() const override;

    QString version() const override;

    QString description() const override;

    QString copyrightYears() const override;

    QList<PluginAuthor> pluginAuthors() const override;

    ReverseGeocodingRunner *newRunner() const override;

    bool canWork() const override;
};

}

#endif
