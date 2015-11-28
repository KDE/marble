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
    return "openlocation-code-search";
}

QString OpenLocationCodeSearchPlugin::version() const
{
    return "1.0";
}

QString OpenLocationCodeSearchPlugin::description() const
{
    return tr( "Decodes an Open Location Code and creates a placemark with the respective coordinates" );
}

QString OpenLocationCodeSearchPlugin::copyrightYears() const
{
    return "2015";
}

QList<PluginAuthor> OpenLocationCodeSearchPlugin::pluginAuthors() const
{
    return QList<PluginAuthor>()
            << PluginAuthor( QString::fromUtf8( "Constantin Mihalache" ), "mihalache.c94@gmail.com" );
}

SearchRunner* OpenLocationCodeSearchPlugin::newRunner() const
{
    return new OpenLocationCodeSearchRunner;
}

}

Q_EXPORT_PLUGIN2( OpenLocationCodeSearchPlugin, Marble::OpenLocationCodeSearchPlugin )

#include "moc_OpenLocationCodeSearchPlugin.cpp"
