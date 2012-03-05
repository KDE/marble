//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011 Thibaut Gridel <tgridel@free.fr>

#include "ShpPlugin.h"
#include "ShpRunner.h"

namespace Marble
{

ShpPlugin::ShpPlugin( QObject *parent ) : RunnerPlugin( parent )
{
    setCapabilities( Parsing );
    setName( tr( "Shp File Parser" ) );
    setNameId( "Shp" );
    setDescription( tr( "Create GeoDataDocument from Shp Files" ) );
    setGuiString( tr( "Shp Parser" ) );
}

QString ShpPlugin::version() const
{
    return "1.0";
}

QString ShpPlugin::copyrightYears() const
{
    return "2011";
}

QList<PluginAuthor> ShpPlugin::pluginAuthors() const
{
    return QList<PluginAuthor>()
            << PluginAuthor( "Thibaut Gridel", "tgridel@free.fr" );
}

MarbleAbstractRunner* ShpPlugin::newRunner() const
{
    return new ShpRunner;
}

}

Q_EXPORT_PLUGIN2( ShpPlugin, Marble::ShpPlugin )

#include "ShpPlugin.moc"
