//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2013 Mohammed Nafees <nafees.technocool@gmail.com>

#ifndef MARBLE_GPSBABEL_PLUGIN_H
#define MARBLE_GPSBABEL_PLUGIN_H

#include "ParseRunnerPlugin.h"

namespace Marble
{

class GpsbabelPlugin : public ParseRunnerPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.kde.marble.GpsbabelPlugin")
    Q_INTERFACES( Marble::ParseRunnerPlugin )

public:
    explicit GpsbabelPlugin( QObject *parent = 0 );

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

#endif // MARBLE_GPSBABEL_PLUGIN_H
