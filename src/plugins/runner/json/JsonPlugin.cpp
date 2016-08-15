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
    return QStringLiteral("GeoJSON");
}

QString JsonPlugin::version() const
{
    return QStringLiteral("1.0");
}

QString JsonPlugin::description() const
{
    return tr( "Create GeoDataDocument from GeoJSON Files" );
}

QString JsonPlugin::copyrightYears() const
{
    return QStringLiteral("2012");
}

QVector<PluginAuthor> JsonPlugin::pluginAuthors() const
{
    return QVector<PluginAuthor>()
            << PluginAuthor(QStringLiteral("Ander Pijoan"), QStringLiteral("ander.pijoan@deusto.es"));
}

QString JsonPlugin::fileFormatDescription() const
{
    return tr( "GeoJSON" );
}

QStringList JsonPlugin::fileExtensions() const
{
    return QStringList() << QStringLiteral("json") << QStringLiteral("geojson");
}

ParsingRunner* JsonPlugin::newRunner() const
{
    return new JsonRunner;
}

}

#include "moc_JsonPlugin.cpp"

