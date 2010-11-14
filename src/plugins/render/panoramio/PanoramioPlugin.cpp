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

using namespace Marble;

PanoramioPlugin::PanoramioPlugin()
{  
    setNameId( "panoramio" );
}
     
void PanoramioPlugin::initialize()
{
    setModel( new PanoramioModel( this ) );
    setNumberOfWidgets( numberOfImagesPerFetch );
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
    return QIcon();
}
Q_EXPORT_PLUGIN2(PanoramioPlugin, Marble::PanoramioPlugin)

#include "PanoramioPlugin.moc"
