//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010      Dennis Nienhüser <earthwings@gentoo.org>
//


#ifndef MARBLE_HOSTIPPLUGIN_H
#define MARBLE_HOSTIPPLUGIN_H

#include "SearchRunnerPlugin.h"

namespace Marble
{

class HostipPlugin : public SearchRunnerPlugin
{
    Q_OBJECT
    Q_INTERFACES( Marble::SearchRunnerPlugin )

public:
    explicit HostipPlugin( QObject *parent = 0 );

    QString name() const;

    QString guiString() const;

    QString nameId() const;

    QString version() const;

    QString description() const;

    QString copyrightYears() const;

    QList<PluginAuthor> pluginAuthors() const;

    virtual MarbleAbstractRunner* newRunner() const;

    bool canWork() const;
};

}

#endif
