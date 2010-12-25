//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010 Utku Aydin <utkuaydin34@gmail.com>
//

#include "EarthquakePlugin.h"
#include "EarthquakeModel.h"

namespace Marble {

EarthquakePlugin::EarthquakePlugin()
    : m_isInitialized( false )
{
    setNameId( "earthquake" );
    setEnabled( true ); // Plugin is enabled by default
    setVisible( false ); // Plugin is invisible by default
}

void EarthquakePlugin::initialize()
{
    setModel( new EarthquakeModel( pluginManager(), this ) );
    setNumberOfItems( numberOfItemsOnScreen );
    m_isInitialized = true;
}

bool EarthquakePlugin::isInitialized() const
{
    return m_isInitialized;
}

QString EarthquakePlugin::name() const
{
    return tr( "Earthquakes" );
}

QString EarthquakePlugin::guiString() const
{
    return tr( "&Earthquakes" );
}

QString EarthquakePlugin::description() const
{
    return tr( "Shows earthquakes on the map." );
}

QIcon EarthquakePlugin::icon() const
{
    return QIcon();
}

}

Q_EXPORT_PLUGIN2( EarthquakePlugin, Marble::EarthquakePlugin )

#include "EarthquakePlugin.moc"
