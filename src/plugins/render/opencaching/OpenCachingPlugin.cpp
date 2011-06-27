//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011 Daniel Marth <danielmarth@gmx.at>
//

#include "OpenCachingPlugin.h"
#include "OpenCachingModel.h"
#include "ui_OpenCachingConfigWidget.h"

#include <QtGui/QPushButton>

namespace Marble {

OpenCachingPlugin::OpenCachingPlugin()
    : m_isInitialized( false ),
      m_aboutDialog( 0 ),
      m_configDialog( 0 )
{
    setNameId( "opencaching" );
    setEnabled( true );  // Plugin is enabled by default
    setVisible( false ); // Plugin is invisible by default
    connect( this, SIGNAL( settingsChanged( QString ) ),
             this, SLOT( updateSettings() ) );
}

void OpenCachingPlugin::initialize()
{
    OpenCachingModel *model = new OpenCachingModel( pluginManager(), this );
    setModel( model );
    setNumberOfItems( numberOfItemsOnScreen );
    readSettings();
    m_isInitialized = true;
}

bool OpenCachingPlugin::isInitialized() const
{
    return m_isInitialized;
}

QString OpenCachingPlugin::name() const
{
    return tr( "OpenCaching" );
}

QString OpenCachingPlugin::guiString() const
{
    return tr( "&OpenCaching" );
}

QString OpenCachingPlugin::description() const
{
    return tr( "Shows caches from OpenCaching.de on the screen." );
}

QIcon OpenCachingPlugin::icon() const
{
    return QIcon();
}

QDialog* OpenCachingPlugin::aboutDialog()
{
    if ( !m_aboutDialog )
    {
        // Initializing about dialog
        m_aboutDialog = new PluginAboutDialog();
        m_aboutDialog->setName( "OpenCaching Plugin" );
        m_aboutDialog->setVersion( "0.1" );
        // FIXME: Can we store this string for all of Marble
        m_aboutDialog->setAboutText( tr( "<br />(c) 2009, 2010 The Marble Project <br /><br /><a href=\"http://edu.kde.org/marble\">http://edu.kde.org/marble</a>" ) );
        QList<Author> authors;
        Author daniel;

        daniel.name = QString::fromUtf8( "Daniel Marth" );
        daniel.task = tr( "Developer" );
        daniel.email= tr( "danielmarth@gmx.at" );
        authors.append( daniel );
        m_aboutDialog->setAuthors( authors );
        m_aboutDialog->setDataText( tr( "Cache positions by opencaching.de." ) );

        m_aboutDialog->setLicense( PluginAboutDialog::License_LGPL_V2 );
    }
    return m_aboutDialog;
}

QDialog *OpenCachingPlugin::configDialog()
{
    if ( !m_configDialog ) {
        // Initializing configuration dialog
        m_configDialog = new QDialog();
        m_ui = new Ui::OpenCachingConfigWidget;
        m_ui->setupUi( m_configDialog );
        readSettings();
        connect( m_ui->m_buttonBox, SIGNAL( accepted() ),
                 SLOT( writeSettings() ) );
        connect( m_ui->m_buttonBox, SIGNAL( rejected() ),
                 SLOT( readSettings() ) );
        connect( m_ui->m_buttonBox->button( QDialogButtonBox::Reset ), SIGNAL( clicked () ),
                 SLOT( restoreDefaultSettings() ) );
        QPushButton *applyButton = m_ui->m_buttonBox->button( QDialogButtonBox::Apply );
        connect( applyButton, SIGNAL( clicked() ),
                 SLOT( writeSettings() ) );
    }
    return m_configDialog;
}

QHash<QString,QVariant> OpenCachingPlugin::settings() const
{
    return m_settings;
}

void OpenCachingPlugin::setSettings( QHash<QString,QVariant> settings )
{
  /*
    if ( !settings.contains( "numResults" ) ) {
        settings.insert( "numResults", numberOfItemsOnScreen );
    }
    if ( !settings.contains( "minMagnitude" ) ) {
        settings.insert( "minMagnitude", 0.0 );
    }
    if ( !settings.contains( "startDate" ) ) {
        settings.insert( "startDate", QDateTime::fromString( "2006-02-04", "yyyy-MM-dd" ) );
    }
    if ( !settings.contains( "endDate" ) ) {
        settings.insert( "endDate", QDateTime::currentDateTime() );
    }

    m_settings = settings;
    readSettings();
    emit settingsChanged( nameId() );
    */
}

void OpenCachingPlugin::readSettings()
{
  /*
    if ( !m_configDialog ) {
        return;
    }

    m_ui->m_numResults->setValue( m_settings.value( "numResults" ).toInt() );
    m_ui->m_minMagnitude->setValue( m_settings.value( "minMagnitude" ).toDouble() );
    m_ui->m_startDate->setDateTime( m_settings.value( "startDate" ).toDateTime() );
    m_ui->m_endDate->setDateTime( m_settings.value( "endDate" ).toDateTime() );
    m_ui->m_startDate->setMaximumDateTime( m_ui->m_endDate->dateTime() );
    */
}

void OpenCachingPlugin::writeSettings()
{
  /*
    Q_ASSERT( m_configDialog );
    m_settings.insert( "numResults", m_ui->m_numResults->value() );
    m_settings.insert( "minMagnitude", m_ui->m_minMagnitude->value() );
    m_settings.insert( "startDate", m_ui->m_startDate->dateTime() );
    m_settings.insert( "endDate", m_ui->m_endDate->dateTime() );

    emit settingsChanged( nameId() );
    */
}

void OpenCachingPlugin::updateSettings()
{
  /*
    OpenCachingModel *OpenCachingModel = dynamic_cast<OpenCachingModel *>( model() );
    if( OpenCachingModel ) {
        OpenCachingModel = new OpenCachingModel( pluginManager(), this );
        Q_ASSERT( m_configDialog );
        OpenCachingModel->setNumResults( m_ui->m_numResults->value() );
        OpenCachingModel->setMinMagnitude( m_ui->m_minMagnitude->value() );
        OpenCachingModel->setEndDate( m_ui->m_endDate->dateTime() );
        OpenCachingModel->setStartDate( m_ui->m_startDate->dateTime() );
        setModel( OpenCachingModel );
    }
    */
}

}

Q_EXPORT_PLUGIN2( OpenCachingPlugin, Marble::OpenCachingPlugin )

#include "OpenCachingPlugin.moc"
