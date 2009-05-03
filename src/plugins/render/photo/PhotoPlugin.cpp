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
#include "PhotoPlugin.h"

#include "PhotoPluginModel.h"

// Marble
#include <QtCore/QDebug>

using namespace Marble;

PhotoPlugin::PhotoPlugin()
{  
    setNameId( "photo" );
}
     
void PhotoPlugin::initialize() {
    qDebug() << "PhotoPlugin: Initialize";
    setModel( new PhotoPluginModel( this ) );
    setNumberOfWidgets( numberOfImagesPerFetch );
}

QString PhotoPlugin::name() const {
    return tr( "Photos" );
}

QString PhotoPlugin::guiString() const {
    return tr( "&Photos" );
}
   
QString PhotoPlugin::description() const {
    return tr( "Automatically downloads images from around the world in preference to their popularity" );
}
    
QIcon PhotoPlugin::icon() const {
    return QIcon();
}
Q_EXPORT_PLUGIN2(PhotoPlugin, Marble::PhotoPlugin)

#include "PhotoPlugin.moc"
