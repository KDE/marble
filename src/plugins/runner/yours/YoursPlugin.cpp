//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010      Dennis Nienhüser <nienhueser@kde.org>
//

#include "YoursPlugin.h"
#include "YoursRunner.h"

namespace Marble
{

YoursPlugin::YoursPlugin( QObject *parent ) :
    RoutingRunnerPlugin( parent )
{
    setSupportedCelestialBodies(QStringList(QStringLiteral("earth")));
    setCanWorkOffline( false );
    setStatusMessage( tr ( "This service requires an Internet connection." ) );
}

QString YoursPlugin::name() const
{
    return tr( "Yours Routing" );
}

QString YoursPlugin::guiString() const
{
    return tr( "Yours" );
}

QString YoursPlugin::nameId() const
{
    return QStringLiteral("yours");
}

QString YoursPlugin::version() const
{
    return QStringLiteral("1.0");
}

QString YoursPlugin::description() const
{
    return tr( "Worldwide routing using a YOURS server" );
}

QString YoursPlugin::copyrightYears() const
{
    return QStringLiteral("2010");
}

QVector<PluginAuthor> YoursPlugin::pluginAuthors() const
{
    return QVector<PluginAuthor>()
            << PluginAuthor(QStringLiteral("Dennis Nienhüser"), QStringLiteral("nienhueser@kde.org"));
}

RoutingRunner *YoursPlugin::newRunner() const
{
    return new YoursRunner;
}

bool YoursPlugin::supportsTemplate( RoutingProfilesModel::ProfileTemplate profileTemplate ) const
{
    return profileTemplate == RoutingProfilesModel::CarFastestTemplate;
}

}

#include "moc_YoursPlugin.cpp"
