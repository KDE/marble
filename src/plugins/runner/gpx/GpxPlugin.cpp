//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011 Thibaut Gridel <tgridel@free.fr>

#include "GpxPlugin.h"
#include "GpxRunner.h"

namespace Marble
{

GpxPlugin::GpxPlugin( QObject *parent ) :
    ParseRunnerPlugin( parent )
{
}

QString GpxPlugin::name() const
{
    return tr( "Gpx File Parser" );
}

QString GpxPlugin::nameId() const
{
    return "Gpx";
}

QString GpxPlugin::version() const
{
    return "1.0";
}

QString GpxPlugin::description() const
{
    return tr( "Create GeoDataDocument from Gpx Files" );
}

QString GpxPlugin::copyrightYears() const
{
    return "2011";
}

QList<PluginAuthor> GpxPlugin::pluginAuthors() const
{
    return QList<PluginAuthor>()
            << PluginAuthor( "Thibaut Gridel", "tgridel@free.fr" );
}

QString GpxPlugin::fileFormatDescription() const
{
    return tr( "GPS Data" );
}

QStringList GpxPlugin::fileExtensions() const
{
    return QStringList() << "gpx";
}

ParsingRunner* GpxPlugin::newRunner() const
{
    return new GpxRunner;
}

}

Q_EXPORT_PLUGIN2( GpxPlugin, Marble::GpxPlugin )

#include "GpxPlugin.moc"
