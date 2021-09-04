// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2010 Dennis Nienhüser <nienhueser@kde.org>
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
