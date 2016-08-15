//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2015      Constantin Mihalache <mihalache.c94@gmail.com>
//

#include "OpenLocationCodeSearchPlugin.h"
#include "OpenLocationCodeSearchRunner.h"

namespace Marble {

OpenLocationCodeSearchPlugin::OpenLocationCodeSearchPlugin( QObject *parent ):
    SearchRunnerPlugin( parent )
{

}

QString OpenLocationCodeSearchPlugin::name() const
{
    return tr( "Open Location Code Search" );
}

QString OpenLocationCodeSearchPlugin::guiString() const
{
    return tr( "Open Location Code" );
}

QString OpenLocationCodeSearchPlugin::nameId() const
{
    return QStringLiteral("openlocation-code-search");
}

QString OpenLocationCodeSearchPlugin::version() const
{
    return QStringLiteral("1.0");
}

QString OpenLocationCodeSearchPlugin::description() const
{
    return tr( "Decodes an Open Location Code and creates a placemark with the respective coordinates" );
}

QString OpenLocationCodeSearchPlugin::copyrightYears() const
{
    return QStringLiteral("2015");
}

QVector<PluginAuthor> OpenLocationCodeSearchPlugin::pluginAuthors() const
{
    return QVector<PluginAuthor>()
            << PluginAuthor(QStringLiteral("Constantin Mihalache"), QStringLiteral("mihalache.c94@gmail.com"));
}

SearchRunner* OpenLocationCodeSearchPlugin::newRunner() const
{
    return new OpenLocationCodeSearchRunner;
}

}

#include "moc_OpenLocationCodeSearchPlugin.cpp"
