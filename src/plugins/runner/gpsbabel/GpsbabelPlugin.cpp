//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2013 Mohammed Nafees <nafees.technocool@gmail.com>

#include "GpsbabelPlugin.h"
#include "GpsbabelRunner.h"

namespace Marble
{

GpsbabelPlugin::GpsbabelPlugin( QObject *parent ) :
    ParseRunnerPlugin( parent )
{
}

QString GpsbabelPlugin::name() const
{
    return tr( "GPSBabel NMEA File Parser" );
}

QString GpsbabelPlugin::nameId() const
{
    return QStringLiteral("GPSBabel");
}

QString GpsbabelPlugin::version() const
{
    return QStringLiteral("1.0");
}

QString GpsbabelPlugin::description() const
{
    return tr( "Allows loading of NMEA files through GPSBabel." );
}

QString GpsbabelPlugin::copyrightYears() const
{
    return QStringLiteral("2013");
}

QVector<PluginAuthor> GpsbabelPlugin::pluginAuthors() const
{
    return QVector<PluginAuthor>()
            << PluginAuthor(QStringLiteral("Mohammed Nafees"), QStringLiteral("nafees.technocool@gmail.com"));
}

QString GpsbabelPlugin::fileFormatDescription() const
{
    return tr( "GPSBabel files" );
}

QStringList GpsbabelPlugin::fileExtensions() const
{
    return QStringList()
        << QStringLiteral("nmea")
        << QStringLiteral("igc")
        << QStringLiteral("tiger")
        << QStringLiteral("ov2")
        << QStringLiteral("garmin")
        << QStringLiteral("csv")
        << QStringLiteral("magellan");
}

ParsingRunner* GpsbabelPlugin::newRunner() const
{
    return new GpsbabelRunner;
}

}

#include "moc_GpsbabelPlugin.cpp"
