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
    setStatusMessage( tr ( "This service requires an Internet connection." ) );
}

QString YoursPlugin::name() const
{
    return tr( "Yours" );
}

QString YoursPlugin::guiString() const
{
    return tr( "Yours Routing" );
}

QString YoursPlugin::nameId() const
{
    return "yours";
}

QString YoursPlugin::version() const
{
    return "1.0";
}

QString YoursPlugin::description() const
{
    return tr( "Worldwide routing using a YOURS server" );
}

QString YoursPlugin::copyrightYears() const
{
    return "2010";
}

QList<PluginAuthor> YoursPlugin::pluginAuthors() const
{
    return QList<PluginAuthor>()
            << PluginAuthor( QString::fromUtf8( "Dennis Nienhüser" ), "earthwings@gentoo.org" );
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
