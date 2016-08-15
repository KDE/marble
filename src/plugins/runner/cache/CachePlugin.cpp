//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011 Thibaut Gridel <tgridel@free.fr>

#include "CachePlugin.h"
#include "CacheRunner.h"

namespace Marble
{

CachePlugin::CachePlugin( QObject *parent ) :
    ParseRunnerPlugin( parent )
{
}

QString CachePlugin::name() const
{
    return tr( "Cache File Parser" );
}

QString CachePlugin::nameId() const
{
    return QStringLiteral("Cache");
}

QString CachePlugin::version() const
{
    return QStringLiteral("1.0");
}

QString CachePlugin::description() const
{
    return tr( "Create GeoDataDocument from Cache Files" );
}

QString CachePlugin::copyrightYears() const
{
    return QStringLiteral("2011");
}

QVector<PluginAuthor> CachePlugin::pluginAuthors() const
{
    return QVector<PluginAuthor>()
            << PluginAuthor(QStringLiteral("Thibaut Gridel"), QStringLiteral("tgridel@free.fr"));
}

QString CachePlugin::fileFormatDescription() const
{
    return tr( "Marble Cache Files" );
}

QStringList CachePlugin::fileExtensions() const
{
    return QStringList(QStringLiteral("cache"));
}

ParsingRunner* CachePlugin::newRunner() const
{
    return new CacheRunner;
}

}

#include "moc_CachePlugin.cpp"
