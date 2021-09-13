// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2012 Dennis Nienhüser <nienhueser@kde.org>
//

#include "OSRMPlugin.h"
#include "OSRMRunner.h"

namespace Marble
{

OSRMPlugin::OSRMPlugin( QObject *parent ) :
    RoutingRunnerPlugin( parent )
{
    setSupportedCelestialBodies(QStringList(QStringLiteral("earth")));
    setCanWorkOffline( false );
    setStatusMessage( tr ( "This service requires an Internet connection." ) );
}

QString OSRMPlugin::name() const
{
    return tr( "Open Source Routing Machine (OSRM) Routing" );
}

QString OSRMPlugin::guiString() const
{
    return tr( "OSRM" );
}

QString OSRMPlugin::nameId() const
{
    return QStringLiteral("osrm");
}

QString OSRMPlugin::version() const
{
    return QStringLiteral("1.0");
}

QString OSRMPlugin::description() const
{
    return tr( "Worldwide routing using project-osrm.org" );
}

QString OSRMPlugin::copyrightYears() const
{
    return QStringLiteral("2012");
}

QVector<PluginAuthor> OSRMPlugin::pluginAuthors() const
{
    return QVector<PluginAuthor>()
            << PluginAuthor(QStringLiteral("Dennis Nienhüser"), QStringLiteral("nienhueser@kde.org"));
}

RoutingRunner *OSRMPlugin::newRunner() const
{
    return new OSRMRunner;
}

bool OSRMPlugin::supportsTemplate( RoutingProfilesModel::ProfileTemplate profileTemplate ) const
{
    return profileTemplate == RoutingProfilesModel::CarFastestTemplate;
}

}

#include "moc_OSRMPlugin.cpp"
