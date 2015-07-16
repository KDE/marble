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
#include "PanoramioPlugin.h"

#include "PanoramioModel.h"

#include "MarbleWidget.h"

using namespace Marble;

PanoramioPlugin::PanoramioPlugin( const MarbleModel *marbleModel ) :
    AbstractDataPlugin( marbleModel )
{
}

QString Marble::PanoramioPlugin::nameId() const
{
    return "panoramio";
}

void PanoramioPlugin::initialize()
{
    setModel( new PanoramioModel( marbleModel(), this ) );
    setNumberOfItems( numberOfImagesPerFetch );
}

QString PanoramioPlugin::name() const
{
    return tr( "Panoramio Photos" );
}

QString PanoramioPlugin::guiString() const
{
    return tr( "&Panoramio" );
}

QString PanoramioPlugin::description() const
{
    return tr( "Automatically downloads images from around the world in preference to their popularity" );
}

QIcon PanoramioPlugin::icon() const
{
    return QIcon( ":/icons/panoramio.png" );
}


QString Marble::PanoramioPlugin::version() const
{
    return "0.1";
}

QString PanoramioPlugin::copyrightYears() const
{
    return "2009, 2014";
}

QList<PluginAuthor> PanoramioPlugin::pluginAuthors() const
{
    return QList<PluginAuthor>() << PluginAuthor( "Bastian Holst", "bastianholst@gmx.de" );
}

bool PanoramioPlugin::eventFilter(QObject *object, QEvent *event)
{
    if ( isInitialized() ) {
        Q_ASSERT( dynamic_cast<PanoramioModel *>( model() ) != 0 );

        PanoramioModel *photoPluginModel = static_cast<PanoramioModel *>( model() );
        MarbleWidget *widget = dynamic_cast<MarbleWidget *>( object );
        if ( widget ) {
            photoPluginModel->setMarbleWidget( widget );
        }
    }

    return AbstractDataPlugin::eventFilter( object, event );
}

Q_EXPORT_PLUGIN2(PanoramioPlugin, Marble::PanoramioPlugin)

#include "moc_PanoramioPlugin.cpp"
