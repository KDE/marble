//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011 Thibaut Gridel <tgridel@free.fr>

#include "GpxPlugin.h"
#include "GpxRunner.h"

namespace Marble
{

GpxPlugin::GpxPlugin( QObject *parent ) : RunnerPlugin( parent )
{
    setCapabilities( Parsing );
    setName( tr( "Gpx File Parser" ) );
    setNameId( "Gpx" );
    setDescription( tr( "Create GeoDataDocument from Gpx Files" ) );
    setGuiString( tr( "Gpx Parser" ) );
}

QString GpxPlugin::version() const
{
    return "1.0";
}

QString GpxPlugin::copyrightYears() const
{
    return "2011";
}

QList<PluginAuthor> GpxPlugin::pluginAuthors() const
{
    return QList<PluginAuthor>()
            << PluginAuthor( "Thibaut Gridel", "tgridel@free.fr" );
}

MarbleAbstractRunner* GpxPlugin::newRunner() const
{
    return new GpxRunner;
}

}

Q_EXPORT_PLUGIN2( GpxPlugin, Marble::GpxPlugin )

#include "GpxPlugin.moc"
