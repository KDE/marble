//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010      Dennis Nienhüser <earthwings@gentoo.org>
//

#include "NominatimPlugin.h"
#include "OsmNominatimRunner.h"

namespace Marble
{

NominatimPlugin::NominatimPlugin( QObject *parent ) : RunnerPlugin( parent )
{
    setCapabilities( Search | ReverseGeocoding );
    setSupportedCelestialBodies( QStringList() << "earth" );
    setCanWorkOffline( false );
    setName( tr( "OSM Nominatim" ) );
    setNameId( "nominatim" );
    setDescription( tr( "Searches for placemarks using the OpenStreetMap Nominatim service" ) );
    setGuiString( tr( "OpenStreetMap Nominatim Search" ) );
}

QString NominatimPlugin::version() const
{
    return "1.0";
}

QString NominatimPlugin::copyrightYears() const
{
    return "2010";
}

QList<PluginAuthor> NominatimPlugin::pluginAuthors() const
{
    return QList<PluginAuthor>()
            << PluginAuthor( QString::fromUtf8( "Dennis Nienhüser" ), "earthwings@gentoo.org" );
}

MarbleAbstractRunner* NominatimPlugin::newRunner() const
{
    return new OsmNominatimRunner;
}

}

Q_EXPORT_PLUGIN2( NominatimPlugin, Marble::NominatimPlugin )

#include "NominatimPlugin.moc"
