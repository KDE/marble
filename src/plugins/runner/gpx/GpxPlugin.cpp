// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2011 Thibaut Gridel <tgridel@free.fr>

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
    return QStringLiteral("Gpx");
}

QString GpxPlugin::version() const
{
    return QStringLiteral("1.0");
}

QString GpxPlugin::description() const
{
    return tr( "Create GeoDataDocument from Gpx Files" );
}

QString GpxPlugin::copyrightYears() const
{
    return QStringLiteral("2011");
}

QVector<PluginAuthor> GpxPlugin::pluginAuthors() const
{
    return QVector<PluginAuthor>()
            << PluginAuthor(QStringLiteral("Thibaut Gridel"), QStringLiteral("tgridel@free.fr"));
}

QString GpxPlugin::fileFormatDescription() const
{
    return tr( "GPS Data" );
}

QStringList GpxPlugin::fileExtensions() const
{
    return QStringList(QStringLiteral("gpx"));
}

ParsingRunner* GpxPlugin::newRunner() const
{
    return new GpxRunner;
}

}

#include "moc_GpxPlugin.cpp"
