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
    : AbstractDataPlugin( 0 ),
      m_ui( 0 ),
      m_configDialog( 0 )
{
}

EarthquakePlugin::EarthquakePlugin( const MarbleModel *marbleModel )
    : AbstractDataPlugin( marbleModel ),
      m_isInitialized( false ),
      m_ui( 0 ),
      m_configDialog( 0 ),
      m_minMagnitude( 0.0 ),
      m_startDate( QDateTime::fromString( "2006-02-04", "yyyy-MM-dd" ) ),
      m_endDate( QDateTime::currentDateTime() )
{
    setEnabled( true ); // Plugin is enabled by default
    setVisible( false ); // Plugin is invisible by default
    connect( this, SIGNAL( settingsChanged( QString ) ),
             this, SLOT( updateModel() ) );
}

void EarthquakePlugin::initialize()
{
    EarthquakeModel *model = new EarthquakeModel( pluginManager(), this );
    setModel( model );
    setNumberOfItems( 20 );
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

QString EarthquakePlugin::nameId() const
{
    return "earthquake";
}

QString EarthquakePlugin::version() const
{
    return "1.0";
}

QString EarthquakePlugin::description() const
{
    return tr( "Shows earthquakes on the map." );
}

QString EarthquakePlugin::copyrightYears() const
{
    return "2010, 2011";
}

QList<PluginAuthor> EarthquakePlugin::pluginAuthors() const
{
    return QList<PluginAuthor>()
            << PluginAuthor( QString::fromUtf8( "Utku Aydın" ), "utkuaydin34@gmail.com" )
            << PluginAuthor( QString::fromUtf8( "Daniel Marth" ), "danielmarth@gmx.at" );
}

QIcon EarthquakePlugin::icon() const
{
    return QIcon();
}

QDialog *EarthquakePlugin::configDialog()
{
    if ( !m_configDialog ) {
        // Initializing configuration dialog
        m_configDialog = new QDialog();
        m_ui = new Ui::EarthquakeConfigWidget;
        m_ui->setupUi( m_configDialog );
        m_ui->m_numResults->setRange( 1, numberOfItems() );
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
        connect( this, SIGNAL( settingsChanged( QString ) ),
                 this, SLOT( readSettings() ) );
    }
    return m_configDialog;
}

QHash<QString,QVariant> EarthquakePlugin::settings() const
{
    QHash<QString, QVariant> settings;

    settings.insert( "numResults", numberOfItems() );
    settings.insert( "minMagnitude", m_minMagnitude );
    settings.insert( "startDate", m_startDate );
    settings.insert( "endDate", m_endDate );

    return settings;
}

void EarthquakePlugin::setSettings( const QHash<QString,QVariant> &settings )
{
    setNumberOfItems( settings.value( "numResults", 20 ).toInt() );
    m_minMagnitude = settings.value( "minMagnitude", 0.0 ).toReal();
    m_startDate = settings.value( "startDate", QDateTime::fromString( "2006-02-04", "yyyy-MM-dd" ) ).toDateTime();
    m_endDate = settings.value( "endDate", QDateTime::currentDateTime() ).toDateTime();

    emit settingsChanged( nameId() );
}

void EarthquakePlugin::readSettings()
{
    Q_ASSERT( m_configDialog );

    m_ui->m_numResults->setValue( numberOfItems() );
    m_ui->m_minMagnitude->setValue( m_minMagnitude );
    m_ui->m_startDate->setDateTime( m_startDate );
    m_ui->m_endDate->setDateTime( m_endDate );
    m_ui->m_startDate->setMaximumDateTime( m_ui->m_endDate->dateTime() );
}

void EarthquakePlugin::writeSettings()
{
    Q_ASSERT( m_configDialog );

    setNumberOfItems( m_ui->m_numResults->value() );
    m_minMagnitude = m_ui->m_minMagnitude->value();
    m_startDate = m_ui->m_startDate->dateTime();
    m_endDate = m_ui->m_endDate->dateTime();

    emit settingsChanged( nameId() );
}

void EarthquakePlugin::updateModel()
{
    if( model() ) {
        EarthquakeModel *const earthquakeModel = new EarthquakeModel( pluginManager(), this );
        earthquakeModel->setMinMagnitude( m_minMagnitude );
        earthquakeModel->setStartDate( m_startDate );
        earthquakeModel->setEndDate( m_endDate );
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
