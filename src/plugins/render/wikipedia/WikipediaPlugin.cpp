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

// Marble
#include "WikipediaModel.h"
#include "PluginAboutDialog.h"
#include "MarbleDirs.h"

using namespace Marble;

WikipediaPlugin::WikipediaPlugin()
    : m_icon()
{
    setNameId( "wikipedia" );
    
    // Plugin is enabled by default
    setEnabled( true );
    // Plugin is not visible by default
    setVisible( false );

    m_aboutDialog = new PluginAboutDialog();
    m_aboutDialog->setName( "Wikipedia Plugin" );
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
    m_aboutDialog->setDataText( tr( "Geo positions by geonames.org\nTexts by wikipedia.org" ) );
    m_icon.addFile( MarbleDirs::path( "svg/wikipedia.svg" ) );
    m_aboutDialog->setPixmap( m_icon.pixmap( 62, 53 ) );
}

WikipediaPlugin::~WikipediaPlugin() {
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
    return m_icon;
}

QDialog *WikipediaPlugin::aboutDialog() const {
    return m_aboutDialog;
}

Q_EXPORT_PLUGIN2(WikipediaPlugin, Marble::WikipediaPlugin)

#include "WikipediaPlugin.moc"
