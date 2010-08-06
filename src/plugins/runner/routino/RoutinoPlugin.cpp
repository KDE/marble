//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010      Dennis Nienhüser <earthwings@gentoo.org>
//

#include "RoutinoPlugin.h"
#include "RoutinoRunner.h"

namespace Marble
{

RoutinoPlugin::RoutinoPlugin( QObject *parent ) : RunnerPlugin( parent )
{
    setCapabilities( Routing );
    setSupportedCelestialBodies( QStringList() << "earth" );
    setCanWorkOffline( true );
    setName( tr( "Routino" ) );
    setNameId( "routino" );
    setDescription( tr( "Retrieves routes from routino" ) );
    setGuiString( tr( "Routino Routing" ) );
}

MarbleAbstractRunner* RoutinoPlugin::newRunner() const
{
    return new RoutinoRunner;
}

}

Q_EXPORT_PLUGIN2( RoutinoPlugin, Marble::RoutinoPlugin )

#include "RoutinoPlugin.moc"
