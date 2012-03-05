//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010      Dennis Nienhüser <earthwings@gentoo.org>
//

#include "LocalDatabasePlugin.h"
#include "LocalDatabaseRunner.h"

namespace Marble
{

LocalDatabasePlugin::LocalDatabasePlugin( QObject *parent ) : RunnerPlugin( parent )
{
    setCapabilities( Search );
    setName( tr( "Local Database" ) );
    setNameId( "localdatabase" );
    setDescription( tr( "Searches the internal Marble database for placemarks" ) );
    setGuiString( tr( "Local Database Search" ) );
}

QString LocalDatabasePlugin::version() const
{
    return "1.0";
}

QString LocalDatabasePlugin::copyrightYears() const
{
    return "2010";
}

QList<PluginAuthor> LocalDatabasePlugin::pluginAuthors() const
{
    return QList<PluginAuthor>()
            << PluginAuthor( QString::fromUtf8( "Dennis Nienhüser" ), "earthwings@gentoo.org" );
}

MarbleAbstractRunner* LocalDatabasePlugin::newRunner() const
{
    return new LocalDatabaseRunner;
}

}

Q_EXPORT_PLUGIN2( LocalDatabasePlugin, Marble::LocalDatabasePlugin )

#include "LocalDatabasePlugin.moc"
