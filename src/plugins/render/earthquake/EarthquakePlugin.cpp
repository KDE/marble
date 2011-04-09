//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011 Utku Aydın <utkuaydin34@gmail.com>
//

#include "EarthquakePlugin.h"
#include "EarthquakeModel.h"
#include "ui_EarthquakeConfigWidget.h"

#include <QtGui/QPushButton>
#include <QtGui/QSlider>

namespace Marble {

EarthquakePlugin::EarthquakePlugin()
    : m_isInitialized( false ),
      m_aboutDialog( 0 ),
      m_configDialog( 0 )
{
    setNameId( "earthquake" );
    setEnabled( true ); // Plugin is enabled by default
    setVisible( false ); // Plugin is invisible by default
    connect( this, SIGNAL( settingsChanged( QString ) ),
             this, SLOT( updateSettings() ) );
}

void EarthquakePlugin::initialize()
{
    EarthquakeModel *model = new EarthquakeModel( pluginManager(), this );
    setModel( model );
    setNumberOfItems( numberOfItemsOnScreen );
    readSettings();
    m_isInitialized = true;
}

bool EarthquakePlugin::isInitialized() const
{
    return m_isInitialized;
}

QString EarthquakePlugin::name() const
{
    return tr( "Earthquakes" );
}

QString EarthquakePlugin::guiString() const
{
    return tr( "&Earthquakes" );
}

QString EarthquakePlugin::description() const
{
    return tr( "Shows earthquakes on the map." );
}

QIcon EarthquakePlugin::icon() const
{
    return QIcon();
}

QDialog* EarthquakePlugin::aboutDialog()
{
    if ( !m_aboutDialog )
    {
        // Initializing about dialog
        m_aboutDialog = new PluginAboutDialog();
        m_aboutDialog->setName( "Earthquake Plugin" );
        m_aboutDialog->setVersion( "0.1" );
        // FIXME: Can we store this string for all of Marble
        m_aboutDialog->setAboutText( tr( "<br />(c) 2009, 2010 The Marble Project <br /><br /><a href=\"http://edu.kde.org/marble\">http://edu.kde.org/marble</a>" ) );
        QList<Author> authors;
        Author utku, daniel;
        
        utku.name = QString::fromUtf8( "Utku Aydın" );
        utku.task = tr( "Developer" );
        utku.email = "utkuaydin34@gmail.com";
        authors.append( utku );
        
        daniel.name = QString::fromUtf8( "Daniel Marth" );
        daniel.task = tr( "Configuration Plugin" );
        authors.append( daniel );
        m_aboutDialog->setAuthors( authors );
        
        m_aboutDialog->setLicense( PluginAboutDialog::License_LGPL_V2 );
    }
    return m_aboutDialog;
}

QDialog *EarthquakePlugin::configDialog()
{
    if ( !m_configDialog ) {
        // Initializing configuration dialog
        m_configDialog = new QDialog();
        m_ui = new Ui::EarthquakeConfigWidget;
        m_ui->setupUi( m_configDialog );
        m_ui->m_numResults->setRange( 1, numberOfItemsOnScreen );
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
        connect( m_ui->m_endDate, SIGNAL( dateTimeChanged ( const QDateTime& ) ),
                 SLOT( validateDateRange() ) );
    }
    return m_configDialog;
}

QHash<QString,QVariant> EarthquakePlugin::settings() const
{
    return m_settings;
}

void EarthquakePlugin::setSettings( QHash<QString,QVariant> settings )
{
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
}

void EarthquakePlugin::readSettings()
{
    if ( !m_configDialog ) {
        return;
    }

    m_ui->m_numResults->setValue( m_settings.value( "numResults" ).toInt() );
    m_ui->m_minMagnitude->setValue( m_settings.value( "minMagnitude" ).toDouble() );
    m_ui->m_startDate->setDateTime( m_settings.value( "startDate" ).toDateTime() );
    m_ui->m_endDate->setDateTime( m_settings.value( "endDate" ).toDateTime() );
    m_ui->m_startDate->setMaximumDateTime( m_ui->m_endDate->dateTime() );
}

void EarthquakePlugin::writeSettings()
{
    Q_ASSERT( m_configDialog );
    m_settings.insert( "numResults", m_ui->m_numResults->value() );
    m_settings.insert( "minMagnitude", m_ui->m_minMagnitude->value() );
    m_settings.insert( "startDate", m_ui->m_startDate->dateTime() );
    m_settings.insert( "endDate", m_ui->m_endDate->dateTime() );

    emit settingsChanged( nameId() );
}

void EarthquakePlugin::updateSettings()
{
    EarthquakeModel *earthquakeModel = dynamic_cast<EarthquakeModel *>( model() );
    if( earthquakeModel ) {
        earthquakeModel = new EarthquakeModel( pluginManager(), this );
        Q_ASSERT( m_configDialog );
        earthquakeModel->setNumResults( m_ui->m_numResults->value() );
        earthquakeModel->setMinMagnitude( m_ui->m_minMagnitude->value() );
        earthquakeModel->setEndDate( m_ui->m_endDate->dateTime() );
        earthquakeModel->setStartDate( m_ui->m_startDate->dateTime() );
        setModel( earthquakeModel );
    }
}

void EarthquakePlugin::validateDateRange()
{
    Q_ASSERT( m_configDialog );
    if( m_ui->m_startDate->dateTime() > m_ui->m_endDate->dateTime() ) {
        m_ui->m_startDate->setDateTime( m_ui->m_endDate->dateTime() );
    }
    m_ui->m_startDate->setMaximumDateTime( m_ui->m_endDate->dateTime() );
}

}

Q_EXPORT_PLUGIN2( EarthquakePlugin, Marble::EarthquakePlugin )

#include "EarthquakePlugin.moc"
