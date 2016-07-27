//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2016  Friedrich W. H. Kossebau <kossebau@kde.org>
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
    Q_INTERFACES( Marble::SearchRunnerPlugin )

public:
    explicit GeoUriPlugin(QObject *parent = nullptr);

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
