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
 
using namespace Marble;
 
OpenDesktopPlugin::OpenDesktopPlugin() : m_isInitialized(false)
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
// Because we want to create a plugin, we have to do the following line.
Q_EXPORT_PLUGIN2( OpenDesktopPlugin, Marble::OpenDesktopPlugin )
 
#include "OpenDesktopPlugin.moc"
