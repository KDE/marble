//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010      Dennis Nienhüser <earthwings@gentoo.org>
//

#include "GosmorePlugin.h"
#include "GosmoreRunner.h"
#include "MarbleDirs.h"

#include <QtCore/QDir>
#include <QtCore/QFileInfo>

namespace Marble
{

GosmorePlugin::GosmorePlugin( QObject *parent ) : RunnerPlugin( parent )
{
    setCapabilities( ReverseGeocoding | Routing );
    setSupportedCelestialBodies( QStringList() << "earth" );
    setCanWorkOffline( true );
    setName( tr( "Gosmore" ) );
    setNameId( "gosmore" );
    setDescription( tr( "Retrieves routes from gosmore" ) );
    setGuiString( tr( "Gosmore Routing" ) );
}

QString GosmorePlugin::version() const
{
    return "1.0";
}

QString GosmorePlugin::copyrightYears() const
{
    return "2010";
}

QList<PluginAuthor> GosmorePlugin::pluginAuthors() const
{
    return QList<PluginAuthor>()
            << PluginAuthor( QString::fromUtf8( "Dennis Nienhüser" ), "earthwings@gentoo.org" );
}

MarbleAbstractRunner* GosmorePlugin::newRunner() const
{
    return new GosmoreRunner;
}

bool GosmorePlugin::canWork( Capability capability ) const
{
    if ( supports( capability ) ) {
        QDir mapDir( MarbleDirs::localPath() + "/maps/earth/gosmore/" );
        QFileInfo mapFile = QFileInfo ( mapDir, "gosmore.pak" );
        return mapFile.exists();
    } else {
        return false;
    }
}

bool GosmorePlugin::supportsTemplate( RoutingProfilesModel::ProfileTemplate profileTemplate ) const
{
    return profileTemplate == RoutingProfilesModel::CarFastestTemplate;
}

}

Q_EXPORT_PLUGIN2( GosmorePlugin, Marble::GosmorePlugin )

#include "GosmorePlugin.moc"
