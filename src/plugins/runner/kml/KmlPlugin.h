//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011 Thibaut Gridel <tgridel@free.fr>

#ifndef MARBLEKMLPLUGIN_H
#define MARBLEKMLPLUGIN_H

#include "ParseRunnerPlugin.h"

namespace Marble
{

class KmlPlugin : public ParseRunnerPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.kde.marble.KmlPlugin")
    Q_INTERFACES( Marble::ParseRunnerPlugin )

public:
    explicit KmlPlugin( QObject *parent = 0 );

    QString name() const;

    QString nameId() const;

    QString version() const;

    QString description() const;

    QString copyrightYears() const;

    QVector<PluginAuthor> pluginAuthors() const override;

    QString fileFormatDescription() const;

    QStringList fileExtensions() const;

    virtual ParsingRunner* newRunner() const;
};

}
#endif // MARBLEKMLPLUGIN_H
