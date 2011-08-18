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

#include "PluginAboutDialog.h"

#include "SatellitesModel.h"

const int timeBetweenDownloads = 6*60*60*1000; //in milliseconds

namespace Marble
{

SatellitesPlugin::SatellitesPlugin()
    : AbstractDataPlugin(),
     m_aboutDialog(0),
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

QDialog *SatellitesPlugin::aboutDialog()
{
    if ( !m_aboutDialog ) {
        m_aboutDialog = new PluginAboutDialog();
        m_aboutDialog->setName( "Satellites Plugin" );
        m_aboutDialog->setVersion( "0.1" );
        m_aboutDialog->setAboutText( tr( "<br />(c) 2011 The Marble Project<br /><br /><a href=\"http://edu.kde.org/marble\">http://edu.kde.org/marble</a>" ) );
        QList<Author> authors;
        Author gmartres;
        gmartres.name = "Guillaume Martres";
        gmartres.task = tr( "Developer" );
        gmartres.email = "smarter@ubuntu.com";
        authors.append( gmartres );
        m_aboutDialog->setAuthors( authors );
        m_aboutDialog->setDataText( tr( "Satellites orbital elements from <a href=\"http://www.celestrak.com\">http://www.celestrak.com</a>" ) );
        m_aboutDialog->setPixmap( icon().pixmap( 62, 53 ) );
    }
    return m_aboutDialog;
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
