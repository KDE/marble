//
// This file is part of the Marble Desktop Globe.
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

MarbleAbstractRunner* NominatimPlugin::newRunner() const
{
    return new OsmNominatimRunner;
}

}

Q_EXPORT_PLUGIN2( NominatimPlugin, Marble::NominatimPlugin )

#include "NominatimPlugin.moc"
