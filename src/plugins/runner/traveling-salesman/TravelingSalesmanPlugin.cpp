//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010      Dennis Nienhüser <nienhueser@kde.org>
//

#include "TravelingSalesmanPlugin.h"
#include "TravelingSalesmanRunner.h"

namespace Marble
{

TravelingSalesmanPlugin::TravelingSalesmanPlugin( QObject *parent ) : RunnerPlugin( parent )
{
    setCapabilities( Routing );
    setSupportedCelestialBodies(QStringList(QStringLiteral("earth")));
    setCanWorkOffline( true );
    setName( tr( "Traveling Salesman" ) );
    setNameId( "travelingsalesman" );
    setDescription( tr( "Retrieves routes from traveling salesman" ) );
    setGuiString( tr( "Traveling Salesman Routing" ) );
}

MarbleAbstractRunner* TravelingSalesmanPlugin::newRunner() const
{
    return new TravelingSalesmanRunner;
}

}

#include "moc_TravelingSalesmanPlugin.cpp"
