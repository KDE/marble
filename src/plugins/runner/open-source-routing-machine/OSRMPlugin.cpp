//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2012      Dennis Nienhüser <earthwings@gentoo.org>
//

#include "OSRMPlugin.h"
#include "OSRMRunner.h"

namespace Marble
{

OSRMPlugin::OSRMPlugin( QObject *parent ) : RunnerPlugin( parent )
{
    setCapabilities( Routing );
    setSupportedCelestialBodies( QStringList() << "earth" );
    setCanWorkOffline( false );
    setStatusMessage( tr ( "This service requires an Internet connection." ) );
}

QString OSRMPlugin::name() const
{
    return tr( "OSRM" );
}

QString OSRMPlugin::guiString() const
{
    return tr( "Open Source Routing Machine (OSRM) Routing" );
}

QString OSRMPlugin::nameId() const
{
    return "osrm";
}

QString OSRMPlugin::version() const
{
    return "1.0";
}

QString OSRMPlugin::description() const
{
    return tr( "Worldwide routing using project-osrm.org" );
}

QString OSRMPlugin::copyrightYears() const
{
    return "2012";
}

QList<PluginAuthor> OSRMPlugin::pluginAuthors() const
{
    return QList<PluginAuthor>()
            << PluginAuthor( QString::fromUtf8( "Dennis Nienhüser" ), "earthwings@gentoo.org" );
}

MarbleAbstractRunner* OSRMPlugin::newRunner() const
{
    return new OSRMRunner;
}

bool OSRMPlugin::supportsTemplate( RoutingProfilesModel::ProfileTemplate profileTemplate ) const
{
    return profileTemplate == RoutingProfilesModel::CarFastestTemplate;
}

}

Q_EXPORT_PLUGIN2( OSRMPlugin, Marble::OSRMPlugin )

#include "OSRMPlugin.moc"
