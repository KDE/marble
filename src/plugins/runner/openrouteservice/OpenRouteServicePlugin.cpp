//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010      Dennis Nienhüser <earthwings@gentoo.org>
//

#include "OpenRouteServicePlugin.h"
#include "OpenRouteServiceRunner.h"

namespace Marble
{

OpenRouteServicePlugin::OpenRouteServicePlugin( QObject *parent ) : RunnerPlugin( parent )
{
    setCapabilities( Routing );
    setSupportedCelestialBodies( QStringList() << "earth" );
    setCanWorkOffline( false );
    setName( "OpenRouteService" );
    setNameId( "openrouteservice" );
    setDescription( "Retrieves routes from openrouteservice.org" );
    setGuiString( "OpenRouteService Routing" );
}

MarbleAbstractRunner* OpenRouteServicePlugin::newRunner() const
{
    return new OpenRouteServiceRunner;
}

}

Q_EXPORT_PLUGIN2( OpenRouteServicePlugin, Marble::OpenRouteServicePlugin )

#include "OpenRouteServicePlugin.moc"
