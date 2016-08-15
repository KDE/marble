//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011 Thibaut Gridel <tgridel@free.fr>

#include "OsmPlugin.h"
#include "OsmRunner.h"

namespace Marble
{

OsmPlugin::OsmPlugin( QObject *parent ) :
    ParseRunnerPlugin( parent )
{
}

QString OsmPlugin::name() const
{
    return tr( "Osm File Parser" );
}

QString OsmPlugin::nameId() const
{
    return QStringLiteral("Osm");
}

QString OsmPlugin::version() const
{
    return QStringLiteral("1.0");
}

QString OsmPlugin::description() const
{
    return tr( "Create GeoDataDocument from Osm Files" );
}

QString OsmPlugin::copyrightYears() const
{
    return QStringLiteral("2011, 2016");
}

QVector<PluginAuthor> OsmPlugin::pluginAuthors() const
{
    return QVector<PluginAuthor>()
            << PluginAuthor(QStringLiteral("Thibaut Gridel"), QStringLiteral("tgridel@free.fr"))
            << PluginAuthor(QStringLiteral("Dennis Nienhüser"), QStringLiteral("nienhueser@kde.org"));
}

QString OsmPlugin::fileFormatDescription() const
{
    return tr( "OpenStreetMap Data" );
}

QStringList OsmPlugin::fileExtensions() const
{
    return QStringList() << QStringLiteral("osm") << QStringLiteral("osm.zip") << QStringLiteral("o5m");
}

ParsingRunner* OsmPlugin::newRunner() const
{
    return new OsmRunner;
}

}

#include "moc_OsmPlugin.cpp"
