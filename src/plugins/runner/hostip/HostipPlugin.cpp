//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010      Dennis Nienhüser <earthwings@gentoo.org>
//

#include "HostipPlugin.h"
#include "HostipRunner.h"

namespace Marble
{

HostipPlugin::HostipPlugin( QObject *parent ) : RunnerPlugin( parent )
{
    setCapabilities( Search );
    setSupportedCelestialBodies( QStringList() << "earth" );
    setCanWorkOffline( false );
    setName( tr( "Hostip.info" ) );
    setNameId( "hostip" );
    setDescription( tr( "Host name and IP geolocation search using the hostip.info service" ) );
    setGuiString( tr( "Hostip.info Search")  );
}

MarbleAbstractRunner* HostipPlugin::newRunner() const
{
    return new HostipRunner;
}

}

Q_EXPORT_PLUGIN2( HostipPlugin, Marble::HostipPlugin )

#include "HostipPlugin.moc"
