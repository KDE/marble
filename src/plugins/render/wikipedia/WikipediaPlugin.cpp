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
#include "WikipediaPlugin.h"

// Marble
#include "ui_WikipediaConfigWidget.h"
#include "WikipediaModel.h"
#include "PluginAboutDialog.h"
#include "MarbleDirs.h"
#include "MarbleDebug.h"

// Qt
#include <QtGui/QPushButton>
#include <QtGui/QLabel>

using namespace Marble;
/* TRANSLATOR Marble::WikipediaPlugin */

const quint32 maximumNumberOfItems = 99;

WikipediaPlugin::WikipediaPlugin()
    : m_isInitialized( false ),
      m_icon(),
      m_aboutDialog( 0 ),
      ui_configWidget( 0 ),
      m_configDialog( 0 ),
      m_settings()
{
    setNameId( "wikipedia" );
    
    // Plugin is enabled by default
    setEnabled( true );
    // Plugin is not visible by default
    setVisible( false );
    
    connect( this, SIGNAL( settingsChanged( QString ) ),
             this, SLOT( updateSettings() ) );
    connect( this, SIGNAL( changedNumberOfItems( quint32 ) ),
             this, SLOT( checkNumberOfItems( quint32 ) ) );
     
    setSettings( QHash<QString,QVariant>() );

    m_icon.addFile( MarbleDirs::path( "svg/wikipedia_shadow.svg" ) );
}

WikipediaPlugin::~WikipediaPlugin()
{
    delete m_aboutDialog;
    delete ui_configWidget;
    delete m_configDialog;
}
     
void WikipediaPlugin::initialize()
{
    WikipediaModel *model = new WikipediaModel( pluginManager(), this );
    // Ensure that all settings get forwarded to the model.
    setModel( model );
    updateSettings();
    m_isInitialized = true;
}

bool WikipediaPlugin::isInitialized() const
{
    return m_isInitialized;
}

QString WikipediaPlugin::name() const
{
    return tr( "Wikipedia Articles" );
}

QString WikipediaPlugin::guiString() const
{
    return tr( "&Wikipedia" );
}
   
QString WikipediaPlugin::description() const
{
    return tr( "Automatically downloads Wikipedia articles and shows them on the right position on the map" );
}
    
QIcon WikipediaPlugin::icon() const
{
    return m_icon;
}

QDialog *WikipediaPlugin::aboutDialog()
{
    if ( !m_aboutDialog ) {
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
        m_aboutDialog->setPixmap( m_icon.pixmap( 62, 53 ) );
    }
    return m_aboutDialog;
}

QDialog *WikipediaPlugin::configDialog()
{
    if ( !m_configDialog ) {
        // Initializing configuration dialog
        m_configDialog = new QDialog();
        ui_configWidget = new Ui::WikipediaConfigWidget;
        ui_configWidget->setupUi( m_configDialog );
        readSettings();
        ui_configWidget->m_itemNumberSpinBox->setRange( 0, maximumNumberOfItems );
        connect( ui_configWidget->m_buttonBox, SIGNAL( accepted() ),
                                            SLOT( writeSettings() ) );
        connect( ui_configWidget->m_buttonBox, SIGNAL( rejected() ),
                                            SLOT( readSettings() ) );
        QPushButton *applyButton = ui_configWidget->m_buttonBox->button( QDialogButtonBox::Apply );
        connect( applyButton, SIGNAL( clicked() ),
                this,        SLOT( writeSettings() ) );
    }
    return m_configDialog;
}

QHash<QString,QVariant> WikipediaPlugin::settings() const
{
    return m_settings;
}

void WikipediaPlugin::setSettings( QHash<QString,QVariant> settings )
{
    if ( !settings.contains( "numberOfItems" ) ) {
        settings.insert( "numberOfItems", 15 );
    }
    else if ( settings.value( "numberOfItems" ).toUInt() > maximumNumberOfItems ) {
        settings.insert( "numberOfItems", maximumNumberOfItems );
    }
    
    if ( !settings.contains( "showThumbnails" ) ) {
        settings.insert( "showThumbnails", true );
    }
    
    m_settings = settings;
    readSettings();
    emit settingsChanged( nameId() );
}

void WikipediaPlugin::readSettings()
{    
    if ( !m_configDialog )
        return;
    
    ui_configWidget->m_itemNumberSpinBox
        ->setValue( (int) m_settings.value( "numberOfItems" ).toInt() );
    
    if ( m_settings.value( "showThumbnails" ).toBool() ) {
        ui_configWidget->m_showThumbnailCheckBox->setCheckState( Qt::Checked );
    }
    else {
        ui_configWidget->m_showThumbnailCheckBox->setCheckState( Qt::Unchecked );
    }
}

void WikipediaPlugin::writeSettings()
{
    setNumberOfItems( ui_configWidget->m_itemNumberSpinBox->value() );
    m_settings.insert( "numberOfItems", ui_configWidget->m_itemNumberSpinBox->value() );
    if ( ui_configWidget->m_showThumbnailCheckBox->checkState() == Qt::Checked ) {
        m_settings.insert( "showThumbnails", true );
    }
    else {
        m_settings.insert( "showThumbnails", false );
    }

    emit settingsChanged( nameId() );
}

void WikipediaPlugin::updateSettings()
{
    setNumberOfItems( m_settings.value( "numberOfItems" ).toUInt() );
    
    AbstractDataPluginModel *abstractModel = model();
    if ( abstractModel != 0 ) {
        abstractModel->setItemSettings( m_settings );
    }
    
    WikipediaModel *wikipediaModel = qobject_cast<WikipediaModel*>( model() );
    if ( wikipediaModel ) {
        wikipediaModel->setShowThumbnail( m_settings.value( "showThumbnails" ).toBool() );
    }
}

void WikipediaPlugin::checkNumberOfItems( quint32 number ) {
    if ( number > maximumNumberOfItems ) {
        setNumberOfItems( maximumNumberOfItems );
    }
    else {
        m_settings.insert( "numberOfItems", number );
    }
    
    readSettings();
}

Q_EXPORT_PLUGIN2(WikipediaPlugin, Marble::WikipediaPlugin)

#include "WikipediaPlugin.moc"
