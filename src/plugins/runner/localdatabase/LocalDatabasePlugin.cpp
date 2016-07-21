//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010      Dennis Nienhüser <nienhueser@kde.org>
//

#include "LocalDatabasePlugin.h"
#include "LocalDatabaseRunner.h"

namespace Marble
{

LocalDatabasePlugin::LocalDatabasePlugin( QObject *parent ) :
    SearchRunnerPlugin( parent )
{
}

QString LocalDatabasePlugin::name() const
{
    return tr( "Local Database Search" );
}

QString LocalDatabasePlugin::guiString() const
{
    return tr( "Local Database" );
}

QString LocalDatabasePlugin::nameId() const
{
    return "localdatabase";
}

QString LocalDatabasePlugin::version() const
{
    return "1.0";
}

QString LocalDatabasePlugin::description() const
{
    return tr( "Searches the internal Marble database for placemarks" );
}

QString LocalDatabasePlugin::copyrightYears() const
{
    return "2010";
}

QVector<PluginAuthor> LocalDatabasePlugin::pluginAuthors() const
{
    return QVector<PluginAuthor>()
            << PluginAuthor( QString::fromUtf8( "Dennis Nienhüser" ), "nienhueser@kde.org" );
}

SearchRunner* LocalDatabasePlugin::newRunner() const
{
    return new LocalDatabaseRunner;
}

}

#include "moc_LocalDatabasePlugin.cpp"
