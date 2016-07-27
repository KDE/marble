//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010      Dennis Nienhüser <nienhueser@kde.org>
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
    explicit LatLonPlugin( QObject *parent = 0 );

    QString name() const;

    QString guiString() const;

    QString nameId() const;

    QString version() const;

    QString description() const;

    QString copyrightYears() const;

    QVector<PluginAuthor> pluginAuthors() const override;

    virtual SearchRunner* newRunner() const;
};

}

#endif
