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
#include "PluginAboutDialog.h"
#include "MarbleDirs.h"

// Qt
#include <QtGui/QIcon>

using namespace Marble;

const quint32 numberOfStationsPerFetch = 10;

WeatherPlugin::WeatherPlugin()
    : m_icon()
{
    setNameId( "weather" );
        
    // Plugin is enabled by default
    setEnabled( true );
    // Plugin is not visible by default
    setVisible( false );

    m_aboutDialog = new PluginAboutDialog();
    m_aboutDialog->setName( "Weather Plugin" );
    m_aboutDialog->setVersion( "0.1" );
    // FIXME: Can we store this string for all of Marble
    m_aboutDialog->setAboutText( tr( "<br />(c) 2009 The Marble Project<br /><br /><a href=\"http://edu.kde.org/marble\">http://edu.kde.org/marble</a>" ) );
    QList<Author> authors;
    Author bholst;
    bholst.name = "Bastian Holst";
    bholst.task = tr( "Developer" );
    bholst.email = "bastianholst@gmx.de";
    authors.append( bholst );
    m_aboutDialog->setAuthors( authors );
    m_aboutDialog->setDataText( tr( "Supported by backstage.bbc.co.uk.\nWeather data from UK MET Office" ) );
    m_icon.addFile( MarbleDirs::path( "weather/weather-clear.svgz" ) );
    m_aboutDialog->setPixmap( m_icon.pixmap( 62, 62 ) );
}

WeatherPlugin::~WeatherPlugin() {
    delete m_aboutDialog;
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
    return m_icon;
}

QDialog *WeatherPlugin::aboutDialog() const {
    return m_aboutDialog;
}

Q_EXPORT_PLUGIN2(WeatherPlugin, Marble::WeatherPlugin)

#include "WeatherPlugin.moc"
