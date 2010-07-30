//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010      Dennis Nienhüser <earthwings@gentoo.org>
//

#include "GosmorePlugin.h"
#include "GosmoreRunner.h"

namespace Marble
{

GosmorePlugin::GosmorePlugin( QObject *parent ) : RunnerPlugin( parent )
{
    setCapabilities( Routing );
    setSupportedCelestialBodies( QStringList() << "earth" );
    setCanWorkOffline( true );
    setName( tr( "Gosmore" ) );
    setNameId( "gosmore" );
    setDescription( tr( "Retrieves routes from gosmore" ) );
    setGuiString( tr( "Gosmore Routing" ) );
}

MarbleAbstractRunner* GosmorePlugin::newRunner() const
{
    return new GosmoreRunner;
}

}

Q_EXPORT_PLUGIN2( GosmorePlugin, Marble::GosmorePlugin )

#include "GosmorePlugin.moc"
