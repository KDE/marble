//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010      Dennis Nienhüser <earthwings@gentoo.org>
//

#include "LatLonPlugin.h"
#include "LatLonRunner.h"

namespace Marble
{

LatLonPlugin::LatLonPlugin( QObject *parent ) :
    SearchRunnerPlugin( parent )
{
}

QString LatLonPlugin::name() const
{
    return tr( "Geographic Coordinates Search" );
}

QString LatLonPlugin::guiString() const
{
    return tr( "Geographic Coordinates" );
}

QString LatLonPlugin::nameId() const
{
    return "latlon";
}

QString LatLonPlugin::version() const
{
    return "1.0";
}

QString LatLonPlugin::description() const
{
    return tr( "Direct input of geographic coordinates" );
}

QString LatLonPlugin::copyrightYears() const
{
    return "2010";
}

QList<PluginAuthor> LatLonPlugin::pluginAuthors() const
{
    return QList<PluginAuthor>()
            << PluginAuthor( QString::fromUtf8( "Dennis Nienhüser" ), "earthwings@gentoo.org" );
}

SearchRunner* LatLonPlugin::newRunner() const
{
    return new LatLonRunner;
}

}

Q_EXPORT_PLUGIN2( LatLonPlugin, Marble::LatLonPlugin )

#include "LatLonPlugin.moc"
