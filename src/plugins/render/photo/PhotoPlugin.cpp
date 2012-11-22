//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009      Bastian Holst <bastianholst@gmx.de>
//

// Self
#include "PhotoPlugin.h"

#include "PhotoPluginModel.h"

// Marble
#include "MarbleDebug.h"

using namespace Marble;
/* TRANSLATOR Marble::PhotoPlugin */

PhotoPlugin::PhotoPlugin()
    : AbstractDataPlugin( 0 )
{
}

PhotoPlugin::PhotoPlugin( const MarbleModel *marbleModel )
    : AbstractDataPlugin( marbleModel )
{
    // Plugin is enabled by default
    setEnabled( true );
    // Plugin is not visible by default
    setVisible( false );
}
     
void PhotoPlugin::initialize()
{
    mDebug() << "PhotoPlugin: Initialize";
    setModel( new PhotoPluginModel( pluginManager(), this ) );
    setNumberOfItems( numberOfImagesPerFetch );
}

QString PhotoPlugin::name() const
{
    return tr( "Photos" );
}

QString PhotoPlugin::guiString() const
{
    return tr( "&Photos" );
}

QString PhotoPlugin::nameId() const
{
    return "photo";
}

QString PhotoPlugin::version() const
{
    return "1.0";
}

QString PhotoPlugin::description() const
{
    return tr( "Automatically downloads images from around the world in preference to their popularity" );
}

QString PhotoPlugin::copyrightYears() const
{
    return "2009";
}

QList<PluginAuthor> PhotoPlugin::pluginAuthors() const
{
    return QList<PluginAuthor>()
            << PluginAuthor( "Bastian Holst", "bastianholst@gmx.de" );
}

QIcon PhotoPlugin::icon() const
{
    return QIcon();
}
Q_EXPORT_PLUGIN2(PhotoPlugin, Marble::PhotoPlugin)

#include "PhotoPlugin.moc"
