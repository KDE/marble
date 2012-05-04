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
    return "Osm";
}

QString OsmPlugin::version() const
{
    return "1.0";
}

QString OsmPlugin::description() const
{
    return tr( "Create GeoDataDocument from Osm Files" );
}

QString OsmPlugin::copyrightYears() const
{
    return "2011";
}

QList<PluginAuthor> OsmPlugin::pluginAuthors() const
{
    return QList<PluginAuthor>()
            << PluginAuthor( "Thibaut Gridel", "tgridel@free.fr" );
}

QString OsmPlugin::fileFormatDescription() const
{
    return tr( "OpenStreetMap Data" );
}

QStringList OsmPlugin::fileExtensions() const
{
    return QStringList() << "osm";
}

MarbleAbstractRunner* OsmPlugin::newRunner() const
{
    return new OsmRunner;
}

}

Q_EXPORT_PLUGIN2( OsmPlugin, Marble::OsmPlugin )

#include "OsmPlugin.moc"
