//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010 Utku Aydın <utkuaydin34@gmail.com>
//

#include "OpenDesktopPlugin.h"
#include "OpenDesktopModel.h"
#include "PluginAboutDialog.h"
 
using namespace Marble;
 
OpenDesktopPlugin::OpenDesktopPlugin()
    : m_isInitialized(false),
      m_aboutDialog( 0 )
{
    setNameId( "opendesktop" );
    setEnabled( true ); // Plugin is enabled by default
    setVisible( false ); // Plugin is invisible by default
}
 
void OpenDesktopPlugin::initialize()
{
    setModel( new OpenDesktopModel( pluginManager(), this ) );
    setNumberOfItems( numberOfItemsOnScreen ); // Setting the number of items on the screen.
    m_isInitialized = true;
}
 
bool OpenDesktopPlugin::isInitialized() const
{
    return m_isInitialized;
}
 
QString OpenDesktopPlugin::name() const
{
    return tr( "OpenDesktop Items" );
}
 
QString OpenDesktopPlugin::guiString() const
{
    return tr( "&OpenDesktop Community" );
}

QString OpenDesktopPlugin::description() const
{
    return tr( "Shows OpenDesktop users' avatars and some extra information about them on the map." );
}
 
QIcon OpenDesktopPlugin::icon() const
{
    return QIcon();
}

QDialog *OpenDesktopPlugin::aboutDialog()
{
    if ( !m_aboutDialog ) {
        // Initializing about dialog
        m_aboutDialog = new PluginAboutDialog();
        m_aboutDialog->setName( "OpenDesktop Plugin" );
        m_aboutDialog->setVersion( "0.1" );
        // FIXME: Can we store this string for all of Marble
        m_aboutDialog->setAboutText( tr( "<br />(c) 2010 The Marble Project<br /><br /><a href=\"http://edu.kde.org/marble\">http://edu.kde.org/marble</a>" ) );
        
        QList<Author> authors;
        Author utku;
        utku.name = QString::fromUtf8( "Utku Aydın" );
        utku.task = tr( "Developer" );
        utku.email = "utkuaydin34@gmail.com";
        authors.append( utku );
        m_aboutDialog->setAuthors( authors );
    }
    return m_aboutDialog;
}

// Because we want to create a plugin, we have to do the following line.
Q_EXPORT_PLUGIN2( OpenDesktopPlugin, Marble::OpenDesktopPlugin )
 
#include "OpenDesktopPlugin.moc"
