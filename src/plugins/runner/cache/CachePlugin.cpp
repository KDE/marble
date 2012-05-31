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
    return "Cache";
}

QString CachePlugin::version() const
{
    return "1.0";
}

QString CachePlugin::description() const
{
    return tr( "Create GeoDataDocument from Cache Files" );
}

QString CachePlugin::copyrightYears() const
{
    return "2011";
}

QList<PluginAuthor> CachePlugin::pluginAuthors() const
{
    return QList<PluginAuthor>()
            << PluginAuthor( "Thibaut Gridel", "tgridel@free.fr" );
}

QString CachePlugin::fileFormatDescription() const
{
    return tr( "Marble Cache Files" );
}

QStringList CachePlugin::fileExtensions() const
{
    return QStringList() << "cache";
}

MarbleAbstractRunner* CachePlugin::newRunner() const
{
    return new CacheRunner;
}

}

Q_EXPORT_PLUGIN2( CachePlugin, Marble::CachePlugin )

#include "CachePlugin.moc"
