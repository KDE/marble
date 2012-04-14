//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011 Thibaut Gridel <tgridel@free.fr>

#include "PntPlugin.h"
#include "PntRunner.h"

namespace Marble
{

PntPlugin::PntPlugin( QObject *parent ) :
    ParseRunnerPlugin( parent )
{
}

QString PntPlugin::name() const
{
    return tr( "Pnt File Parser" );
}

QString PntPlugin::guiString() const
{
    return tr( "Pnt Parser" );
}

QString PntPlugin::nameId() const
{
    return "Pnt";
}

QString PntPlugin::version() const
{
    return "1.0";
}

QString PntPlugin::description() const
{
    return tr( "Create GeoDataDocument from Pnt Files" );
}

QString PntPlugin::copyrightYears() const
{
    return "2011";
}

QList<PluginAuthor> PntPlugin::pluginAuthors() const
{
    return QList<PluginAuthor>()
            << PluginAuthor( "Thibaut Gridel", "tgridel@free.fr" );
}

MarbleAbstractRunner* PntPlugin::newRunner() const
{
    return new PntRunner;
}

}

Q_EXPORT_PLUGIN2( PntPlugin, Marble::PntPlugin )

#include "PntPlugin.moc"
