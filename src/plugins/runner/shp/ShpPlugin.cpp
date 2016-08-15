//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011 Thibaut Gridel <tgridel@free.fr>

#include "ShpPlugin.h"
#include "ShpRunner.h"

namespace Marble
{

ShpPlugin::ShpPlugin( QObject *parent ) :
    ParseRunnerPlugin( parent )
{
}

QString ShpPlugin::name() const
{
    return tr( "Shp File Parser" );
}

QString ShpPlugin::nameId() const
{
    return QStringLiteral("Shp");
}

QString ShpPlugin::version() const
{
    return QStringLiteral("1.0");
}

QString ShpPlugin::description() const
{
    return tr( "Create GeoDataDocument from Shp Files" );
}

QString ShpPlugin::copyrightYears() const
{
    return QStringLiteral("2011");
}

QVector<PluginAuthor> ShpPlugin::pluginAuthors() const
{
    return QVector<PluginAuthor>()
            << PluginAuthor(QStringLiteral("Thibaut Gridel"), QStringLiteral("tgridel@free.fr"));
}

QString ShpPlugin::fileFormatDescription() const
{
    return tr( "Shapefile Map Files" );
}

QStringList ShpPlugin::fileExtensions() const
{
    return QStringList(QStringLiteral("shp"));
}

ParsingRunner* ShpPlugin::newRunner() const
{
    return new ShpRunner;
}

}

#include "moc_ShpPlugin.cpp"
