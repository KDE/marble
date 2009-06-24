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

// Qt
#include <QtCore/QDebug>
#include <QtGui/QPushButton>
#include <QtGui/QLabel>

using namespace Marble;

const quint32 maximumNumberOfItems = 99;

WikipediaPlugin::WikipediaPlugin()
    : m_icon()
{
    setNameId( "wikipedia" );
    
    // Plugin is enabled by default
    setEnabled( true );
    // Plugin is not visible by default
    setVisible( false );

    // Initializing about dialog
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

    // Initializing configuration dialog
    m_configDialog = new QDialog();
    ui_configWidget.setupUi( m_configDialog );
    ui_configWidget.m_itemNumberSpinBox->setRange( 0, maximumNumberOfItems );
    connect( ui_configWidget.m_buttonBox, SIGNAL( accepted() ),
                                          SLOT( writeSettings() ) );
    connect( ui_configWidget.m_buttonBox, SIGNAL( rejected() ),
                                          SLOT( readSettings() ) );
    QPushButton *applyButton = ui_configWidget.m_buttonBox->button( QDialogButtonBox::Apply );
    connect( applyButton, SIGNAL( clicked() ),
             this,        SLOT( writeSettings() ) );
    connect( this, SIGNAL( changedNumberOfItems( quint32 ) ),
             this, SLOT( setDialogNumberOfItems( quint32 ) ) );

    readSettings();
}

WikipediaPlugin::~WikipediaPlugin() {
    delete m_aboutDialog;
    delete m_configDialog;
}
     
void WikipediaPlugin::initialize() {
    setModel( new WikipediaModel( this ) );
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

QDialog *WikipediaPlugin::configDialog() const {
    return m_configDialog;
}

void WikipediaPlugin::setShowThumbnails( bool shown ) {
    if ( shown ) {
        ui_configWidget.m_showThumbnailCheckBox->setCheckState( Qt::Checked );
    }
    else {
        ui_configWidget.m_showThumbnailCheckBox->setCheckState( Qt::Unchecked );
    }

    WikipediaModel *wikipediaModel = qobject_cast<WikipediaModel*>( model() );

    if ( wikipediaModel ) {
        wikipediaModel->setShowThumbnail( shown );
    }
}

void WikipediaPlugin::readSettings() {
    setNumberOfItems( 15 );
    setDialogNumberOfItems( 15 );
    setShowThumbnails( true );
}

void WikipediaPlugin::writeSettings() {
    setNumberOfItems( ui_configWidget.m_itemNumberSpinBox->value() );
    if ( ui_configWidget.m_showThumbnailCheckBox->checkState() ) {
        setShowThumbnails( true );
    }
    else {
        setShowThumbnails( false );
    }
}

void WikipediaPlugin::setDialogNumberOfItems( quint32 number ) {
    if ( number <= maximumNumberOfItems ) {
        ui_configWidget.m_itemNumberSpinBox->setValue( (int) number );
    }
    else {
        // Force a the number of items being lower or equal maximumNumberOfItems
        setNumberOfItems( maximumNumberOfItems );
    }
}


Q_EXPORT_PLUGIN2(WikipediaPlugin, Marble::WikipediaPlugin)

#include "WikipediaPlugin.moc"
