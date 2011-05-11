//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010      Dennis Nienhüser <earthwings@gentoo.org>
//

#include "YoursPlugin.h"
#include "YoursRunner.h"

namespace Marble
{

YoursPlugin::YoursPlugin( QObject *parent ) : RunnerPlugin( parent )
{
    setCapabilities( Routing );
    setSupportedCelestialBodies( QStringList() << "earth" );
    setCanWorkOffline( false );
    setName( tr( "Yours" ) );
    setNameId( "yours" );
    setDescription( tr( "Worldwide routing using a YOURS server" ) );
    setStatusMessage( tr ( "This service requires an Internet connection." ) );
    setGuiString( tr( "Yours Routing" ) );
}

MarbleAbstractRunner* YoursPlugin::newRunner() const
{
    return new YoursRunner;
}

bool YoursPlugin::supportsTemplate( RoutingProfilesModel::ProfileTemplate profileTemplate ) const
{
    return profileTemplate == RoutingProfilesModel::CarFastestTemplate;
}

}

Q_EXPORT_PLUGIN2( YoursPlugin, Marble::YoursPlugin )

#include "YoursPlugin.moc"
