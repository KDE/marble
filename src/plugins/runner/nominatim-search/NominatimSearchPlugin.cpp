//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010      Dennis Nienhüser <earthwings@gentoo.org>
// Copyright 2012      Bernhard Beschow <bbeschow@cs.tu-berlin.de>
//

#include "NominatimSearchPlugin.h"
#include "OsmNominatimSearchRunner.h"

namespace Marble
{

NominatimPlugin::NominatimPlugin( QObject *parent ) : RunnerPlugin( parent )
{
    setCapabilities( Search );
    setSupportedCelestialBodies( QStringList() << "earth" );
    setCanWorkOffline( false );
}

QString NominatimPlugin::name() const
{
    return tr( "OpenStreetMap Nominatim Search" );
}

QString NominatimPlugin::guiString() const
{
    return tr( "OpenStreetMap Nominatim" );
}

QString NominatimPlugin::nameId() const
{
    return "nominatim-search";
}

QString NominatimPlugin::version() const
{
    return "1.0";
}

QString NominatimPlugin::description() const
{
    return tr( "Online search for placemarks using the OpenStreetMap Nominatim service" );
}

QString NominatimPlugin::copyrightYears() const
{
    return "2010, 2012";
}

QList<PluginAuthor> NominatimPlugin::pluginAuthors() const
{
    return QList<PluginAuthor>()
            << PluginAuthor( QString::fromUtf8( "Dennis Nienhüser" ), "earthwings@gentoo.org" )
            << PluginAuthor( "Bernhard Beschow", "bbeschow@cs.tu-berlin.de" );
}

MarbleAbstractRunner* NominatimPlugin::newRunner() const
{
    return new OsmNominatimRunner;
}

}

Q_EXPORT_PLUGIN2( NominatimSearchPlugin, Marble::NominatimPlugin )

#include "NominatimSearchPlugin.moc"
