/*
 This file is part of the Marble Virtual Globe.

 This program is free software licensed under the GNU LGPL. You can
 find a copy of this license in LICENSE.txt in the top directory of
 the source code.

 Copyright 2013 Ander Pijoan <ander.pijoan@deusto.es>
*/

#include "JsonPlugin.h"
#include "JsonRunner.h"

namespace Marble
{

JsonPlugin::JsonPlugin( QObject *parent ) :
    ParseRunnerPlugin( parent )
{
}

QString JsonPlugin::name() const
{
    return tr( "GeoJSON File Parser" );
}

QString JsonPlugin::nameId() const
{
    return "GeoJSON";
}

QString JsonPlugin::version() const
{
    return "1.0";
}

QString JsonPlugin::description() const
{
    return tr( "Create GeoDataDocument from GeoJSON Files" );
}

QString JsonPlugin::copyrightYears() const
{
    return "2012";
}

QList<PluginAuthor> JsonPlugin::pluginAuthors() const
{
    return QList<PluginAuthor>()
            << PluginAuthor( "Ander Pijoan", "ander.pijoan@deusto.es" );
}

QString JsonPlugin::fileFormatDescription() const
{
    return tr( "GeoJSON" );
}

QStringList JsonPlugin::fileExtensions() const
{
    return QStringList() << "json";
}

ParsingRunner* JsonPlugin::newRunner() const
{
    return new JsonRunner;
}

}

Q_EXPORT_PLUGIN2( JsonPlugin, Marble::JsonPlugin )

#include "moc_JsonPlugin.cpp"

