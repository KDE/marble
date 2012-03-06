//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2012      Bernhard Beschow <bbeschow@cs.tu-berlin.de>

#include "LogPlugin.h"
#include "LogRunner.h"

namespace Marble
{

LogfilePlugin::LogfilePlugin( QObject *parent ) :
    RunnerPlugin( parent )
{
    setCapabilities( Parsing );
}

QString LogfilePlugin::name() const
{
    return tr( ".log File Parser" );
}

QString LogfilePlugin::guiString() const
{
    return tr( ".log File Parser" );
}

QString LogfilePlugin::nameId() const
{
    return "Log";
}

QString LogfilePlugin::version() const
{
    return "1.0";
}

QString LogfilePlugin::description() const
{
    return tr( "Create GeoDataDocument from TangoGPS .log Files" );
}

QString LogfilePlugin::copyrightYears() const
{
    return "2012";
}

QList<PluginAuthor> LogfilePlugin::pluginAuthors() const
{
    return QList<PluginAuthor>()
            << PluginAuthor( "Bernhard Beschow", "bbeschow@cs.tu-berlin.de" );
}

MarbleAbstractRunner* LogfilePlugin::newRunner() const
{
    return new LogRunner;
}

}

Q_EXPORT_PLUGIN2( LogfilePlugin, Marble::LogfilePlugin )

#include "LogPlugin.moc"
