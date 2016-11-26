//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010      Niko Sams <niko.sams@gmail.com>
//


#include "RoutingProfileSettingsDialog.h"

#include <QStandardItemModel>

#include "MarbleGlobal.h"
#include "RoutingRunnerPlugin.h"
#include "MarbleDebug.h"
#include "RoutingProfilesModel.h"
#include "PluginManager.h"

#include "ui_RoutingProfileSettingsDialog.h"

namespace Marble {


RoutingProfileSettingsDialog::RoutingProfileSettingsDialog( const PluginManager *pluginManager, RoutingProfilesModel *profilesModel, QWidget* parent )
    : QDialog( parent ),
    m_profilesModel ( profilesModel ), m_dialog( 0 ), m_dialogLayout( 0 )
{
    m_ui = new Ui_RoutingProfileSettingsDialog();
    m_ui->setupUi( this );
    bool const smallScreen = MarbleGlobal::getInstance()->profiles() & MarbleGlobal::SmallScreen;
    if ( smallScreen ) {
      setMinimumHeight( 480 );
      m_ui->services->setMinimumWidth( 280 );
      m_ui->buttonBox->hide();
    }

    QList<RoutingRunnerPlugin*> allPlugins = pluginManager->routingRunnerPlugins();
    foreach( RoutingRunnerPlugin* plugin, allPlugins ) {
        m_plugins << plugin;
        RoutingRunnerPlugin::ConfigWidget* configWidget = plugin->configWidget();
        if ( configWidget ) {
            m_configWidgets.insert( plugin, configWidget );
            m_ui->settingsStack->addWidget( configWidget );
        }
    }

    m_servicesModel = new QStandardItemModel;
    m_ui->services->setModel( m_servicesModel );
    connect ( m_ui->services->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)), SLOT(updateConfigWidget()), Qt::QueuedConnection );
    connect ( m_servicesModel, SIGNAL(dataChanged(QModelIndex,QModelIndex)), SLOT(updateConfigWidget()) );

    connect ( m_ui->buttonBox, SIGNAL(accepted()), this, SLOT(accept()) );
    connect ( m_ui->buttonBox, SIGNAL(rejected()), this, SLOT(reject()) );

    connect( m_ui->configureButton, SIGNAL(clicked()), this, SLOT(openConfigDialog()) );
}

RoutingProfileSettingsDialog::~RoutingProfileSettingsDialog()
{
    qDeleteAll( m_configWidgets );
    delete m_ui;
}

void RoutingProfileSettingsDialog::updateConfigWidget( )
{
    QModelIndex current = m_ui->services->selectionModel()->currentIndex();

    if ( !current.isValid() ) {
        m_ui->settingsStack->setEnabled( false );
        return;
    }

    RoutingRunnerPlugin *plugin = m_plugins.at( current.row() );
    QWidget* configWidget = m_configWidgets[plugin];
    if ( configWidget ) {
        bool const smallScreen = MarbleGlobal::getInstance()->profiles() & MarbleGlobal::SmallScreen;
        m_ui->settingsStack->setCurrentWidget( smallScreen ? m_ui->configurePage : configWidget );
        m_ui->descriptionLabel->setText( plugin->description() );
        m_ui->statusLabel->setText( plugin->statusMessage() );
        QStandardItem *item = m_servicesModel->invisibleRootItem()->child( current.row() );
        m_ui->settingsStack->setEnabled( item->checkState() == Qt::Checked );
    } else {
        m_ui->settingsStack->setEnabled( false );
        m_ui->noConfigDescriptionLabel->setText( plugin->description() );
        m_ui->noConfigStatusLabel->setText( plugin->statusMessage() );
        m_ui->settingsStack->setCurrentWidget( m_ui->noConfigAvailablePage );
    }
}

void RoutingProfileSettingsDialog::editProfile( int profileIndex )
{
    QList<RoutingProfile> profiles = m_profilesModel->profiles();
    m_ui->name->setText( profiles.at( profileIndex ).name() );

    m_servicesModel->clear();
    foreach( RoutingRunnerPlugin *plugin,  m_plugins ) {
        QStandardItem *item = new QStandardItem( plugin->guiString() );
        item->setCheckable( true );
        if ( profiles[ profileIndex ].pluginSettings().contains( plugin->nameId() ) ) {
            item->setCheckState( Qt::Checked );
            QHash<QString, QVariant> settings = profiles[ profileIndex ].pluginSettings()[ plugin->nameId() ];
            mDebug() << settings;
            if ( m_configWidgets[plugin] ) {
                m_configWidgets[plugin]->loadSettings( settings );
            }
        }
        m_servicesModel->invisibleRootItem()->appendRow( item );
    }
    m_ui->settingsStack->setCurrentWidget( m_ui->selectServicePage );
    m_ui->settingsStack->setEnabled( false );

    bool const smallScreen = MarbleGlobal::getInstance()->profiles() & MarbleGlobal::SmallScreen;
    // Small screen profile (like maemo 5) does not have the "buttonBox"
    if ( exec() != QDialog::Accepted && !smallScreen ) {
        return;
    }

    m_profilesModel->setProfileName( profileIndex, m_ui->name->text() );

    QHash< QString, QHash< QString, QVariant > > pluginSettings;
    for ( int i=0; i < m_servicesModel->invisibleRootItem()->rowCount(); ++i) {
        QStandardItem *item = m_servicesModel->invisibleRootItem()->child( i );
        if ( item->checkState() == Qt::Checked ) {
            QHash<QString, QVariant> settings;
            if ( m_configWidgets[m_plugins[ i ]] ) {
                settings = m_configWidgets[m_plugins[ i ]]->settings();
            }
            mDebug() << i << m_plugins[ i ]->nameId() << settings;
            pluginSettings.insert( m_plugins[ i ]->nameId(), settings );
        } else {
            pluginSettings.remove( m_plugins[ i ]->nameId() );
        }
    }
    m_profilesModel->setProfilePluginSettings( profileIndex, pluginSettings );
}

void RoutingProfileSettingsDialog::openConfigDialog()
{
    QModelIndex current = m_ui->services->selectionModel()->currentIndex();
    if ( current.isValid() ) {
        RoutingRunnerPlugin *plugin = m_plugins.at( current.row() );

        if ( !m_dialog ) {
            m_dialog = new QDialog( this );

            m_dialogLayout = new QHBoxLayout();
            m_dialogLayout->addWidget( m_configWidgets[plugin] );

            m_dialog->setLayout( m_dialogLayout );
            m_dialog->setMinimumHeight( 480 );
        } else {
            m_dialogLayout->insertWidget( 0, m_configWidgets[plugin] );
        }

        m_configWidgets[plugin]->show();
        m_dialog->setWindowTitle( plugin->guiString() );
        m_dialog->exec();
        m_configWidgets[plugin]->hide();
        m_dialogLayout->removeWidget( m_configWidgets[plugin] );
    }
}

}

#include "moc_RoutingProfileSettingsDialog.cpp"
