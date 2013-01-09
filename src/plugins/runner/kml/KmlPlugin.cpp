//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011 Thibaut Gridel <tgridel@free.fr>

#include "KmlPlugin.h"
#include "KmlRunner.h"

namespace Marble
{

KmlPlugin::KmlPlugin( QObject *parent ) :
    ParseRunnerPlugin( parent )
{
}

QString KmlPlugin::name() const
{
    return tr( "Kml File Parser" );
}

QString KmlPlugin::nameId() const
{
    return "Kml";
}

QString KmlPlugin::version() const
{
    return "1.0";
}

QString KmlPlugin::description() const
{
    return tr( "Create GeoDataDocument from Kml Files" );
}

QString KmlPlugin::copyrightYears() const
{
    return "2011";
}

QList<PluginAuthor> KmlPlugin::pluginAuthors() const
{
    return QList<PluginAuthor>()
            << PluginAuthor( "Thibaut Gridel", "tgridel@free.fr" );
}

QString KmlPlugin::fileFormatDescription() const
{
    return tr( "Google Earth KML" );
}

QStringList KmlPlugin::fileExtensions() const
{
    return QStringList() << "kml";
}

ParsingRunner* KmlPlugin::newRunner() const
{
    return new KmlRunner;
}

}

Q_EXPORT_PLUGIN2( KmlPlugin, Marble::KmlPlugin )

#include "KmlPlugin.moc"
