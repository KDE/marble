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
#include "WikipediaPlugin.h"

#include "WikipediaModel.h"

using namespace Marble;

WikipediaPlugin::WikipediaPlugin()
{
    setNameId( "wikipedia" );
    
    // Plugin is not enabled by default
    setEnabled( false );
}
     
void WikipediaPlugin::initialize() {
    setModel( new WikipediaModel( this ) );
    setNumberOfItems( numberOfArticlesPerFetch );
}

QString WikipediaPlugin::name() const {
    return tr( "Wikipedia Articles" );
}

QString WikipediaPlugin::guiString() const {
    return tr( "&Wikipedia" );
}
   
QString WikipediaPlugin::description() const {
    return tr( "Automatically downloads Wikipedia articles and shows them on the right position on the map" );
}
    
QIcon WikipediaPlugin::icon() const {
    return QIcon();
}
Q_EXPORT_PLUGIN2(WikipediaPlugin, Marble::WikipediaPlugin)

#include "WikipediaPlugin.moc"
