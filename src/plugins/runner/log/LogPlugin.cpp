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
    ParseRunnerPlugin( parent )
{
}

QString LogfilePlugin::name() const
{
    return tr( "TangoGPS Log File Parser" );
}

QString LogfilePlugin::nameId() const
{
    return QStringLiteral("Log");
}

QString LogfilePlugin::version() const
{
    return QStringLiteral("1.0");
}

QString LogfilePlugin::description() const
{
    return tr( "Allows loading of TangoGPS log files." );
}

QString LogfilePlugin::copyrightYears() const
{
    return QStringLiteral("2012");
}

QVector<PluginAuthor> LogfilePlugin::pluginAuthors() const
{
    return QVector<PluginAuthor>()
            << PluginAuthor(QStringLiteral("Bernhard Beschow"), QStringLiteral("bbeschow@cs.tu-berlin.de"));
}

QString LogfilePlugin::fileFormatDescription() const
{
    return tr( "TangoGPS Log Files" );
}

QStringList LogfilePlugin::fileExtensions() const
{
    return QStringList(QStringLiteral("log"));
}

ParsingRunner* LogfilePlugin::newRunner() const
{
    return new LogRunner;
}

}

#include "moc_LogPlugin.cpp"
