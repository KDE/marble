//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2012 Utku Aydın <utkuaydin34@gmail.com>
//

#include "FoursquarePlugin.h"
#include "FoursquareModel.h"

namespace Marble {

FoursquarePlugin::FoursquarePlugin()
    : AbstractDataPlugin( 0 )
{
}

FoursquarePlugin::FoursquarePlugin(const MarbleModel* marbleModel)
    : AbstractDataPlugin(marbleModel),
      m_isInitialized( false )
{
    setEnabled( true ); // Plugin is enabled by default
    setVisible( false ); // Plugin is invisible by default
}

void FoursquarePlugin::initialize()
{
    FoursquareModel *model = new FoursquareModel( pluginManager(), this );
    setModel( model );
    setNumberOfItems( 20 ); // Do we hardcode that?
    m_isInitialized = true;
}

bool FoursquarePlugin::isInitialized() const
{
    return m_isInitialized;
}

QString FoursquarePlugin::name() const
{
    return tr( "Places" );
}

QString FoursquarePlugin::guiString() const
{
    return tr( "&Places" ); // TODO: Check if that ampersand conflicts with another
}

QString FoursquarePlugin::nameId() const
{
    return "foursquare";
}

QString FoursquarePlugin::version() const
{
    return "1.0";
}

QString FoursquarePlugin::description() const
{
    return tr( "Displays trending Foursquare places" );
}

QString FoursquarePlugin::copyrightYears() const
{
    return "2012";
}

QList<PluginAuthor> FoursquarePlugin::pluginAuthors() const
{
    return QList<PluginAuthor>()
            << PluginAuthor( QString::fromUtf8( "Dennis Nienhüser" ), "earthwings@gentoo.org" )
            << PluginAuthor( QString::fromUtf8( "Utku Aydın" ), "utkuaydin34@gmail.com" );
}

QIcon FoursquarePlugin::icon() const
{
    return QIcon();
}

}

Q_EXPORT_PLUGIN2(FoursquarePlugin, Marble::FoursquarePlugin)
#include "FoursquarePlugin.moc"

