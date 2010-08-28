//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010      Dennis Nienhüser <earthwings@gentoo.org>
//

#include "MonavPlugin.h"
#include "MonavRunner.h"

namespace Marble
{

MonavPlugin::MonavPlugin( QObject *parent ) : RunnerPlugin( parent )
{
    setCapabilities( Routing );
    setSupportedCelestialBodies( QStringList() << "earth" );
    setCanWorkOffline( true );
    setName( tr( "Monav" ) );
    setNameId( "monav" );
    setDescription( tr( "Retrieves routes from monav" ) );
    setGuiString( tr( "Monav Routing" ) );
}

MarbleAbstractRunner* MonavPlugin::newRunner() const
{
    return new MonavRunner;
}

}

Q_EXPORT_PLUGIN2( MonavPlugin, Marble::MonavPlugin )

#include "MonavPlugin.moc"
