//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011 Thibaut Gridel <tgridel@free.fr>

#include "KmlPlugin.h"
#include "KmlRunner.h"

namespace Marble
{

KmlPlugin::KmlPlugin( QObject *parent ) : RunnerPlugin( parent )
{
    setCapabilities( Parsing );
    setName( tr( "Kml File Parser" ) );
    setNameId( "Kml" );
    setDescription( tr( "Create GeoDataDocument from Kml Files" ) );
    setGuiString( tr( "Kml Parser" ) );
}

QString KmlPlugin::version() const
{
    return "1.0";
}

QString KmlPlugin::copyrightYears() const
{
    return "2011";
}

QList<PluginAuthor> KmlPlugin::pluginAuthors() const
{
    return QList<PluginAuthor>()
            << PluginAuthor( "Thibaut Gridel", "tgridel@free.fr" );
}

MarbleAbstractRunner* KmlPlugin::newRunner() const
{
    return new KmlRunner;
}

}

Q_EXPORT_PLUGIN2( KmlPlugin, Marble::KmlPlugin )

#include "KmlPlugin.moc"
