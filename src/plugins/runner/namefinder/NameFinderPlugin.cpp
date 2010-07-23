//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010      Dennis Nienhüser <earthwings@gentoo.org>
//

#include "NameFinderPlugin.h"
#include "OnfRunner.h"

namespace Marble
{

NameFinderPlugin::NameFinderPlugin( QObject *parent ) : RunnerPlugin( parent )
{
    setCapabilities( Search );
    setSupportedCelestialBodies( QStringList() << "earth" );
    setCanWorkOffline( false );
    setName( "OSM Namefinder" );
    setNameId( "namefinder" );
    setDescription( "Searches for placemarks using the OpenStreetMap NameFinder service" );
    setGuiString( "OpenStreetMap NameFinder Search" );
}

MarbleAbstractRunner* NameFinderPlugin::newRunner() const
{
    return new OnfRunner;
}

}

Q_EXPORT_PLUGIN2( NameFinderPlugin, Marble::NameFinderPlugin )

#include "NameFinderPlugin.moc"
