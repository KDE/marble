// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2010 Dennis Nienhüser <nienhueser@kde.org>
//


#ifndef MARBLE_LATLONPLUGIN_H
#define MARBLE_LATLONPLUGIN_H

#include "SearchRunnerPlugin.h"

namespace Marble
{

class LatLonPlugin : public SearchRunnerPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.kde.marble.LatLonPlugin")
    Q_INTERFACES( Marble::SearchRunnerPlugin )

public:
    explicit LatLonPlugin( QObject *parent = nullptr );

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
