//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009      Bastian Holst <bastianholst@gmx.de>
//

// Self
#include "WeatherPlugin.h"

// Marble
#include "WeatherModel.h"

using namespace Marble;

const quint32 numberOfStationsPerFetch = 10;

WeatherPlugin::WeatherPlugin() {
    setNameId( "weather" );
        
    // Plugin is enabled by default
    setEnabled( true );
    // Plugin is not visible by default
    setVisible( false );
}

WeatherPlugin::~WeatherPlugin() {
}

void WeatherPlugin::initialize() {
    setModel( new WeatherModel( this ) );
    setNumberOfItems( numberOfStationsPerFetch );
}

QString WeatherPlugin::name() const {
    return tr( "Weather" );
}

QString WeatherPlugin::guiString() const {
    return tr( "&Weather" );
}

QString WeatherPlugin::description() const {
    return tr( "Download weather information from many weather stations all around the world" );
}

QIcon WeatherPlugin::icon() const {
    return QIcon();
}

Q_EXPORT_PLUGIN2(WeatherPlugin, Marble::WeatherPlugin)

#include "WeatherPlugin.moc"
