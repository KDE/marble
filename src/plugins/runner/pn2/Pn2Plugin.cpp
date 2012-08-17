//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2012 Cezar Mocan <mocancezar@gmail.com>

#include "Pn2Plugin.h"
#include "Pn2Runner.h"

namespace Marble
{

Pn2Plugin::Pn2Plugin( QObject *parent ) :
    ParseRunnerPlugin( parent )
{
}

QString Pn2Plugin::name() const
{
    return tr( "Pn2 File Parser" );
}

QString Pn2Plugin::nameId() const
{
    return "Pn2";
}

QString Pn2Plugin::version() const
{
    return "1.0";
}

QString Pn2Plugin::description() const
{
    return tr( "Create GeoDataDocument from Pn2 Files" );
}

QString Pn2Plugin::copyrightYears() const
{
    return "2012";
}

QList<PluginAuthor> Pn2Plugin::pluginAuthors() const
{
    return QList<PluginAuthor>()
            << PluginAuthor( "Cezar Mocan", "mocancezar@gmail.com" );
}

QString Pn2Plugin::fileFormatDescription() const
{
    return tr( "ESRI Shapefiles" );
}

QStringList Pn2Plugin::fileExtensions() const
{
    return QStringList() << "pn2";
}

MarbleAbstractRunner* Pn2Plugin::newRunner() const
{
    return new Pn2Runner;
}

}

Q_EXPORT_PLUGIN2( Pn2Plugin, Marble::Pn2Plugin )

#include "Pn2Plugin.moc"
