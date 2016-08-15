//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010      Dennis Nienhüser <nienhueser@kde.org>
//

#include "HostipPlugin.h"
#include "HostipRunner.h"
#include "MarbleGlobal.h"

namespace Marble
{

HostipPlugin::HostipPlugin( QObject *parent ) :
    SearchRunnerPlugin( parent )
{
    setSupportedCelestialBodies(QStringList(QStringLiteral("earth")));
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
    return QStringLiteral("hostip");
}

QString HostipPlugin::version() const
{
    return QStringLiteral("1.0");
}

QString HostipPlugin::description() const
{
    return tr( "Host name and IP geolocation search using the hostip.info service" );
}

QString HostipPlugin::copyrightYears() const
{
    return QStringLiteral("2010");
}

QVector<PluginAuthor> HostipPlugin::pluginAuthors() const
{
    return QVector<PluginAuthor>()
            << PluginAuthor(QStringLiteral("Dennis Nienhüser"), QStringLiteral("nienhueser@kde.org"));
}

SearchRunner* HostipPlugin::newRunner() const
{
    return new HostipRunner;
}

bool HostipPlugin::canWork() const
{
    bool const smallScreen = MarbleGlobal::getInstance()->profiles() & MarbleGlobal::SmallScreen;
    // Disabled on small screen devices to save resources
    return !smallScreen;
}

}

#include "moc_HostipPlugin.cpp"
