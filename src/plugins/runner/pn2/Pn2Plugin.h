//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2012 Cezar Mocan <mocancezar@gmail.com>

#ifndef MARBLEPN2PLUGIN_H
#define MARBLEPN2PLUGIN_H

#include "ParseRunnerPlugin.h"

namespace Marble
{

class Pn2Plugin : public ParseRunnerPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.kde.marble.Pn2Plugin")
    Q_INTERFACES( Marble::ParseRunnerPlugin )

public:
    explicit Pn2Plugin( QObject *parent = 0 );

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
#endif // MARBLEPN2PLUGIN_H
