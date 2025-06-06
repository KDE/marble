// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2016 Friedrich W. H. Kossebau <kossebau@kde.org>
//

#ifndef MARBLE_GEOURIPLUGIN_H
#define MARBLE_GEOURIPLUGIN_H

#include "SearchRunnerPlugin.h"

namespace Marble
{

class GeoUriPlugin : public SearchRunnerPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.kde.marble.GeoUriPlugin")
    Q_INTERFACES(Marble::SearchRunnerPlugin)

public:
    explicit GeoUriPlugin(QObject *parent = nullptr);

    QString name() const override;

    QString guiString() const override;

    QString nameId() const override;

    QString version() const override;

    QString description() const override;

    QString copyrightYears() const override;

    QList<PluginAuthor> pluginAuthors() const override;

    SearchRunner *newRunner() const override;
};

}

#endif
