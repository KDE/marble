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
    return "GPSBabel";
}

QString GpsbabelPlugin::version() const
{
    return "1.0";
}

QString GpsbabelPlugin::description() const
{
    return tr( "Allows loading of NMEA files through GPSBabel." );
}

QString GpsbabelPlugin::copyrightYears() const
{
    return "2013";
}

QList<PluginAuthor> GpsbabelPlugin::pluginAuthors() const
{
    return QList<PluginAuthor>()
            << PluginAuthor( "Mohammed Nafees", "nafees.technocool@gmail.com" );
}

QString GpsbabelPlugin::fileFormatDescription() const
{
    return tr( "GPSBabel files" );
}

QStringList GpsbabelPlugin::fileExtensions() const
{
    return QStringList() << "nmea" << "igc" << "tiger" << "ov2"
                         << "garmin" << "csv" << "magellan";
}

ParsingRunner* GpsbabelPlugin::newRunner() const
{
    return new GpsbabelRunner;
}

}

Q_EXPORT_PLUGIN2( GpsbabelPlugin, Marble::GpsbabelPlugin )

#include "GpsbabelPlugin.moc"
