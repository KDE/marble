/*
 This file is part of the Marble Virtual Globe.

 This program is free software licensed under the GNU LGPL. You can
 find a copy of this license in LICENSE.txt in the top directory of
 the source code.

 Copyright 2012 Ander Pijoan <ander.pijoan@deusto.es>
*/

#ifndef MARBLE_JSONPLUGIN_H
#define MARBLE_JSONPLUGIN_H

#include "ParseRunnerPlugin.h"

namespace Marble
{

class JsonPlugin : public ParseRunnerPlugin
{
    Q_OBJECT
    Q_INTERFACES( Marble::ParseRunnerPlugin )

public:
    explicit JsonPlugin( QObject *parent = 0 );

    QString name() const;

    QString nameId() const;

    QString version() const;

    QString description() const;

    QString copyrightYears() const;

    QList<PluginAuthor> pluginAuthors() const;

    QString fileFormatDescription() const;

    QStringList fileExtensions() const;

    virtual MarbleAbstractRunner* newRunner() const;
};

}

#endif
