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
}

QString HostipPlugin::name() const
{
    return tr( "Hostip.info Search" );
}

QString HostipPlugin::guiString() const
{
    return tr( "Hostip.info" );
}

QString HostipPlugin::nameId() const
{
    return "hostip";
}

QString HostipPlugin::version() const
{
    return "1.0";
}

QString HostipPlugin::description() const
{
    return tr( "Host name and IP geolocation search using the hostip.info service" );
}

QString HostipPlugin::copyrightYears() const
{
    return "2010";
}

QList<PluginAuthor> HostipPlugin::pluginAuthors() const
{
    return QList<PluginAuthor>()
            << PluginAuthor( QString::fromUtf8( "Dennis Nienhüser" ), "earthwings@gentoo.org" );
}

MarbleAbstractRunner* HostipPlugin::newRunner() const
{
    return new HostipRunner;
}

bool HostipPlugin::canWork( Capability capability ) const
{
    bool const smallScreen = MarbleGlobal::getInstance()->profiles() & MarbleGlobal::SmallScreen;
    // Disabled on small screen devices to save resources
    return supports( capability ) && !smallScreen;
}

}

Q_EXPORT_PLUGIN2( HostipPlugin, Marble::HostipPlugin )

#include "HostipPlugin.moc"
