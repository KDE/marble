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

#include <QPushButton>
#include <QSlider>

#include "MarbleModel.h"

namespace Marble {

EarthquakePlugin::EarthquakePlugin()
    : AbstractDataPlugin( 0 ),
      m_ui( 0 ),
      m_configDialog( 0 ),
      m_maximumNumberOfItems( 100 )
{
}

EarthquakePlugin::EarthquakePlugin( const MarbleModel *marbleModel )
    : AbstractDataPlugin( marbleModel ),
      m_ui( 0 ),
      m_configDialog( 0 ),
      m_minMagnitude( 0.0 ),
      m_startDate( QDateTime::fromString( "2006-02-04", "yyyy-MM-dd" ) ),
      m_endDate( marbleModel->clockDateTime() ),
      m_pastDays( 30 ),
      m_timeRangeNPastDays( true ),
      m_numResults( 20 ),
      m_maximumNumberOfItems( 100 )
{
    setEnabled( true ); // Plugin is enabled by default
    setVisible( false ); // Plugin is invisible by default
    connect( this, SIGNAL(settingsChanged(QString)),
             this, SLOT(updateModel()) );
}

void EarthquakePlugin::initialize()
{
    EarthquakeModel *model = new EarthquakeModel( marbleModel(), this );
    setModel( model );
    setNumberOfItems( m_numResults );
    
    updateModel();
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
    return QStringLiteral("earthquake");
}

QString EarthquakePlugin::version() const
{
    return QStringLiteral("1.0");
}

QString EarthquakePlugin::description() const
{
    return tr( "Shows earthquakes on the map." );
}

QString EarthquakePlugin::copyrightYears() const
{
    return QStringLiteral("2010, 2011");
}

QVector<PluginAuthor> EarthquakePlugin::pluginAuthors() const
{
    return QVector<PluginAuthor>()
            << PluginAuthor(QStringLiteral("Utku Aydın"), QStringLiteral("utkuaydin34@gmail.com"))
            << PluginAuthor(QStringLiteral("Daniel Marth"), QStringLiteral("danielmarth@gmx.at"));
}

QIcon EarthquakePlugin::icon() const
{
    return QIcon(QStringLiteral(":/icons/earthquake.png"));
}

QDialog *EarthquakePlugin::configDialog()
{
    if ( !m_configDialog ) {
        // Initializing configuration dialog
        m_configDialog = new QDialog();
        m_ui = new Ui::EarthquakeConfigWidget;
        m_ui->setupUi( m_configDialog );
        m_ui->m_numResults->setRange( 1, m_maximumNumberOfItems );
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
        connect( m_ui->m_startDate, SIGNAL(dateTimeChanged(QDateTime)),
                 SLOT(validateDateRange()) );
        connect( m_ui->m_endDate, SIGNAL(dateTimeChanged(QDateTime)),
                 SLOT(validateDateRange()) );
        connect( this, SIGNAL(settingsChanged(QString)),
                 this, SLOT(readSettings()) );
    }
    return m_configDialog;
}

QHash<QString,QVariant> EarthquakePlugin::settings() const
{
    QHash<QString, QVariant> settings = AbstractDataPlugin::settings();

    settings.insert(QStringLiteral("numResults"), m_numResults);
    settings.insert(QStringLiteral("minMagnitude"), m_minMagnitude);
    settings.insert(QStringLiteral("startDate"), m_startDate);
    settings.insert(QStringLiteral("endDate"), m_endDate);
    settings.insert(QStringLiteral("pastDays"), m_pastDays);
    settings.insert(QStringLiteral("timeRangeNPastDays"), m_timeRangeNPastDays);
    settings.insert(QStringLiteral("maximumNumberOfItems"), m_maximumNumberOfItems);

    return settings;
}

void EarthquakePlugin::setSettings( const QHash<QString,QVariant> &settings )
{
    AbstractDataPlugin::setSettings( settings );

    m_numResults = settings.value(QStringLiteral("numResults"), 20).toInt();
    m_minMagnitude = settings.value(QStringLiteral("minMagnitude"), 0.0).toReal();
    m_startDate = settings.value(QStringLiteral("startDate"), QDateTime::fromString(QStringLiteral("2006-02-04"), QStringLiteral("yyyy-MM-dd"))).toDateTime();
    m_endDate = settings.value(QStringLiteral("endDate"), marbleModel()->clockDateTime()).toDateTime();
    m_pastDays = settings.value(QStringLiteral("pastDays"), 30).toInt();
    m_timeRangeNPastDays = settings.value(QStringLiteral("timeRangeNPastDays"), true).toBool();
    m_maximumNumberOfItems = settings.value(QStringLiteral("maximumNumberOfItems"), m_maximumNumberOfItems).toInt();
    
    emit settingsChanged( nameId() );
}

void EarthquakePlugin::readSettings()
{
    Q_ASSERT( m_configDialog );

    m_ui->m_numResults->setValue( m_numResults );
    m_ui->m_minMagnitude->setValue( m_minMagnitude );
    m_ui->m_startDate->setDateTime( m_startDate );
    m_ui->m_endDate->setDateTime( m_endDate );
    m_ui->m_startDate->setMaximumDateTime( m_ui->m_endDate->dateTime() );
    m_ui->m_pastDays->setValue(m_pastDays);
    if ( m_timeRangeNPastDays ) {
        m_ui->m_timeRangeNPastDaysRadioButton->setChecked(true);
    } else {
        m_ui->m_timeRangeFromToRadioButton->setChecked(true);
    }
}

void EarthquakePlugin::writeSettings()
{
    Q_ASSERT( m_configDialog );

    m_numResults = m_ui->m_numResults->value();
    setNumberOfItems( m_numResults );
    m_minMagnitude = m_ui->m_minMagnitude->value();
    m_startDate = m_ui->m_startDate->dateTime();
    m_endDate = m_ui->m_endDate->dateTime();
    m_pastDays = m_ui->m_pastDays->value();
    m_timeRangeNPastDays = m_ui->m_timeRangeNPastDaysRadioButton->isChecked();
        
    emit settingsChanged( nameId() );
}

void EarthquakePlugin::updateModel()
{
    if( model() ) {
        EarthquakeModel *const earthquakeModel = static_cast<EarthquakeModel *>( model() );
        earthquakeModel->setMinMagnitude( m_minMagnitude );
        if ( m_timeRangeNPastDays ) {
            QDateTime startdate, enddate;
            
            enddate = marbleModel()->clockDateTime();
            startdate = enddate.addDays(-m_pastDays + 1);
            earthquakeModel->setStartDate( startdate );
            earthquakeModel->setEndDate( enddate );
        } else {
            earthquakeModel->setStartDate( m_startDate );
            earthquakeModel->setEndDate( m_endDate );
        }
        earthquakeModel->clear();
    }
}

void EarthquakePlugin::validateDateRange()
{
    Q_ASSERT( m_configDialog );
    if( m_ui->m_startDate->dateTime() >= m_ui->m_endDate->dateTime() ) {
        
        m_ui->m_startDate->setDateTime( m_ui->m_endDate->dateTime().addDays(-1) );
    }
    m_ui->m_startDate->setMaximumDateTime( m_ui->m_endDate->dateTime().addDays(-1) );
}

}

#include "moc_EarthquakePlugin.cpp"
