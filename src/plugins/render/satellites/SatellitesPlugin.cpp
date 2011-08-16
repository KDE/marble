//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011 Guillaume Martres <smarter@ubuntu.com>
//

#include "SatellitesPlugin.h"

#include "MarbleDebug.h"
#include "MarbleDirs.h"
#include "MarbleModel.h"

#include "SatellitesModel.h"

const int timeBetweenDownloads = 6*60*60*1000; //in milliseconds

namespace Marble
{

SatellitesPlugin::SatellitesPlugin()
    : AbstractDataPlugin(),
     m_isInitialized(false)
{
}

QStringList SatellitesPlugin::backendTypes() const
{
    return QStringList( "satellites" );
}

QString SatellitesPlugin::renderPolicy() const
{
    return QString( "ALWAYS" );
}

QStringList SatellitesPlugin::renderPosition() const
{
    return QStringList( "ORBIT" );
}

QString SatellitesPlugin::name() const
{
    return tr( "Satellites" );
}

QString SatellitesPlugin::guiString() const
{
    return tr( "&Satellites" );
}

QString SatellitesPlugin::nameId() const
{
    return QString( "satellites-plugin" );
}

QString SatellitesPlugin::description() const
{
    return tr( "This plugin displays satellites and their orbits." );
}

QIcon SatellitesPlugin::icon() const
{
    return QIcon();
}

void SatellitesPlugin::initialize()
{
    setModel( new SatellitesModel( pluginManager(), this ) );
    m_isInitialized = true;
}

bool SatellitesPlugin::isInitialized() const
{
    return m_isInitialized;
}

}

Q_EXPORT_PLUGIN2( SatellitesPlugin, Marble::SatellitesPlugin )

#include "SatellitesPlugin.moc"
