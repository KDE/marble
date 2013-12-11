//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2013      Mihail Ivchenko <ematirov@gmail.com>
//

#include "CycleStreetsPlugin.h"
#include "CycleStreetsRunner.h"

#include <QSet>

namespace Marble
{

CycleStreetsPlugin::CycleStreetsPlugin( QObject *parent ) :
    RoutingRunnerPlugin( parent )
{
    setSupportedCelestialBodies( QStringList() << "earth" );
    setCanWorkOffline( false );
    setStatusMessage( tr ( "This service requires an Internet connection." ) );
}

QString CycleStreetsPlugin::name() const
{
    return tr( "CycleStreets Routing" );
}

QString CycleStreetsPlugin::guiString() const
{
    return tr( "CycleStreets" );
}

QString CycleStreetsPlugin::nameId() const
{
    return "cyclestreets";
}

QString CycleStreetsPlugin::version() const
{
    return "1.0";
}

QString CycleStreetsPlugin::description() const
{
    return tr( "Bicycle routing for the United Kingdom using cyclestreets.net" );
}

QString CycleStreetsPlugin::copyrightYears() const
{
    return "2013";
}

QList<PluginAuthor> CycleStreetsPlugin::pluginAuthors() const
{
    return QList<PluginAuthor>()
            << PluginAuthor( QString::fromUtf8( "Mihail Ivchenko" ), "ematirov@gmail.com" );
}

RoutingRunner *CycleStreetsPlugin::newRunner() const
{
    return new CycleStreetsRunner;
}

bool CycleStreetsPlugin::supportsTemplate( RoutingProfilesModel::ProfileTemplate profileTemplate ) const
{
    QSet<RoutingProfilesModel::ProfileTemplate> availableTemplates;
    availableTemplates.insert( RoutingProfilesModel::BicycleTemplate );
    return availableTemplates.contains( profileTemplate );
}

}

Q_EXPORT_PLUGIN2( CycleStreetsPlugin, Marble::CycleStreetsPlugin )

#include "CycleStreetsPlugin.moc"