//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011 Thibaut Gridel <tgridel@free.fr>
// Copyright 2015 Dennis Nienhüser <nienhueser@kde.org>

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
    return tr( "KML File Parser" );
}

QString KmlPlugin::nameId() const
{
    return "Kml";
}

QString KmlPlugin::version() const
{
    return "1.2";
}

QString KmlPlugin::description() const
{
    return tr( "Create GeoDataDocument from KML and KMZ Files" );
}

QString KmlPlugin::copyrightYears() const
{
    return "2011, 2013, 2015";
}

QVector<PluginAuthor> KmlPlugin::pluginAuthors() const
{
    return QVector<PluginAuthor>()
            << PluginAuthor( "Thibaut Gridel", "tgridel@free.fr" )
            << PluginAuthor( "Dennis Nienhüser", "nienhueser@kde.org" );
}

QString KmlPlugin::fileFormatDescription() const
{
    return tr( "Google Earth KML" );
}

QStringList KmlPlugin::fileExtensions() const
{
    return QStringList() << "kml" << "kmz";
}

ParsingRunner* KmlPlugin::newRunner() const
{
    return new KmlRunner;
}

}

#include "moc_KmlPlugin.cpp"
