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

#include <QPushButton>

namespace Marble {

OpenCachingPlugin::OpenCachingPlugin()
    : m_isInitialized( false ),
      m_configDialog( 0 )
{
    setNameId( "opencaching" );
    setVersion( "1.0" );
    setCopyrightYear( 2011 );
    addAuthor( "Daniel Marth", "danielmarth@gmx.at" );
    setDataText( tr( "Cache positions by opencaching.de." ) );

    setEnabled( true );  // Plugin is enabled by default
    setVisible( false ); // Plugin is invisible by default
    connect( this, SIGNAL(settingsChanged(QString)),
             this, SLOT(updateSettings()) );
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

QDialog *OpenCachingPlugin::configDialog()
{
    if ( !m_configDialog ) {
        // Initializing configuration dialog
        m_configDialog = new QDialog();
        m_ui = new Ui::OpenCachingConfigWidget;
        m_ui->setupUi( m_configDialog );
        readSettings();
        connect( m_ui->m_buttonBox, SIGNAL(accepted()),
                 SLOT(writeSettings()) );
        connect( m_ui->m_buttonBox, SIGNAL(rejected()),
                 SLOT(readSettings()) );
        connect( m_ui->m_buttonBox->button( QDialogButtonBox::Reset ), SIGNAL(clicked()),
                 SLOT(restoreDefaultSettings()) );
        QPushButton *applyButton = m_ui->m_buttonBox->button( QDialogButtonBox::Apply );
        connect( applyButton, SIGNAL(clicked()),
                 SLOT(writeSettings()) );
        connect( m_ui->m_endDate, SIGNAL(dateTimeChanged(QDateTime)),
                 SLOT(validateDateRange()) );
        connect( m_ui->m_maxDifficulty, SIGNAL(valueChanged(double)),
                 SLOT(validateDifficultyRange()) );
    }
    return m_configDialog;
}

QHash<QString,QVariant> OpenCachingPlugin::settings() const
{
    return m_settings;
}

void OpenCachingPlugin::setSettings(const QHash<QString, QVariant> &settings)
{
    if ( !settings.contains( "numResults" ) ) {
        settings.insert( "numResults", numberOfItemsOnScreen );
    }
    if ( !settings.contains( "maxDistance" ) ) {
        settings.insert( "maxDistance", 20 );
    }
    if ( !settings.contains( "minDfficulty" ) ) {
        settings.insert( "minDfficulty", 0.0 );
    }
    if ( !settings.contains( "maxDifficulty" ) ) {
        settings.insert( "maxDifficulty", 5.0 );
    }
    if ( !settings.contains( "startDate" ) ) {
        settings.insert( "startDate", QDateTime::fromString( "2006-01-01", "yyyy-MM-dd" ) );
    }
    if ( !settings.contains( "endDate" ) ) {
        settings.insert( "endDate", QDateTime::currentDateTime() );
    }

    m_settings = settings;
    readSettings();
    emit settingsChanged( nameId() );
}

void OpenCachingPlugin::readSettings()
{
    if ( !m_configDialog ) {
        return;
    }

    m_ui->m_numResults->setValue( m_settings.value( "numResults" ).toInt() );
    m_ui->m_minDifficulty->setValue( m_settings.value( "maxDistance" ).toInt() );
    m_ui->m_minDifficulty->setValue( m_settings.value( "minDifficulty" ).toDouble() );
    m_ui->m_maxDifficulty->setValue( m_settings.value( "maxDifficulty" ).toDouble() );
    m_ui->m_startDate->setDateTime( m_settings.value( "startDate" ).toDateTime() );
    m_ui->m_endDate->setDateTime( m_settings.value( "endDate" ).toDateTime() );
    m_ui->m_startDate->setMaximumDateTime( m_ui->m_endDate->dateTime() );
}

void OpenCachingPlugin::writeSettings()
{
    Q_ASSERT( m_configDialog );
    m_settings.insert( "numResults", m_ui->m_numResults->value() );
    m_settings.insert( "maxDistance", m_ui->m_maxDistance->value() );
    m_settings.insert( "minDifficulty", m_ui->m_minDifficulty->value() );
    m_settings.insert( "maxDifficulty", m_ui->m_maxDifficulty->value() );
    m_settings.insert( "startDate", m_ui->m_startDate->dateTime() );
    m_settings.insert( "endDate", m_ui->m_endDate->dateTime() );

    emit settingsChanged( nameId() );
}

void OpenCachingPlugin::updateSettings()
{
    OpenCachingModel *model = dynamic_cast<OpenCachingModel *>( this->model() );
    if( model ) {
        model = new OpenCachingModel( pluginManager(), this );
        Q_ASSERT( m_configDialog );
        model->setNumResults( m_ui->m_numResults->value() );
        model->setMaxDistance( m_ui->m_maxDistance->value () );
        model->setMinDifficulty( m_ui->m_minDifficulty->value() );
        model->setMaxDifficulty( m_ui->m_maxDifficulty->value() );
        model->setEndDate( m_ui->m_endDate->dateTime() );
        model->setStartDate( m_ui->m_startDate->dateTime() );
        setModel( model );
    }
}

void OpenCachingPlugin::validateDateRange()
{
    Q_ASSERT( m_configDialog );
    if( m_ui->m_startDate->dateTime() > m_ui->m_endDate->dateTime() ) {
        m_ui->m_startDate->setDateTime( m_ui->m_endDate->dateTime() );
    }
    m_ui->m_startDate->setMaximumDateTime( m_ui->m_endDate->dateTime() );
}

void OpenCachingPlugin::validateDifficultyRange()
{
    Q_ASSERT( m_configDialog );
    if( m_ui->m_minDifficulty->value() > m_ui->m_maxDifficulty->value() ) {
        m_ui->m_minDifficulty->setValue( m_ui->m_maxDifficulty->value() );
    }
    m_ui->m_minDifficulty->setMaximum( m_ui->m_maxDifficulty->value() );
}

}

#include "moc_OpenCachingPlugin.cpp"
