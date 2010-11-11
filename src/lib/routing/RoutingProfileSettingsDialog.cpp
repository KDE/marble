//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010      Niko Sams <niko.sams@gmail.com>
//


#include "RoutingProfileSettingsDialog.h"

#include <QStandardItemModel>

#include "RunnerPlugin.h"
#include "MarbleDebug.h"
#include "RoutingProfilesModel.h"
#include "PluginManager.h"

#include "ui_RoutingProfileSettingsDialog.h"

namespace Marble {


RoutingProfileSettingsDialog::RoutingProfileSettingsDialog( PluginManager *pluginManager, RoutingProfilesModel *profilesModel, QWidget* parent )
    : QDialog( parent ), m_pluginManager( pluginManager ), m_profilesModel ( profilesModel )
{
    m_ui = new Ui_RoutingProfileSettingsDialog();
    m_ui->setupUi( this );
    bool const smallScreen = MarbleGlobal::getInstance()->profiles() & MarbleGlobal::SmallScreen;
    if ( smallScreen ) {
      setMaximumWidth( 800 );
    }

    QList<RunnerPlugin*> allPlugins = pluginManager->runnerPlugins();
    foreach( RunnerPlugin* plugin, allPlugins ) {
        if ( !plugin->supports( RunnerPlugin::Routing ) ) {
            continue;
        }
        m_plugins << plugin;
        RunnerPlugin::ConfigWidget* configWidget = plugin->configWidget();
        if ( configWidget ) {
            m_configWidgets.insert( plugin, configWidget );
            m_ui->settingsStack->addWidget( configWidget );
        }
    }

    m_servicesModel = new QStandardItemModel;
    m_ui->services->setModel( m_servicesModel );
    connect ( m_ui->services->selectionModel(), SIGNAL( currentChanged( QModelIndex, QModelIndex ) ), SLOT( updateConfigWidget() ), Qt::QueuedConnection );
    connect ( m_servicesModel, SIGNAL( dataChanged( QModelIndex, QModelIndex ) ), SLOT( updateConfigWidget() ) );

    connect ( m_ui->buttonBox, SIGNAL( accepted() ), this, SLOT( accept() ) );
    connect ( m_ui->buttonBox, SIGNAL( rejected() ), this, SLOT( reject() ) );

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
    }

    RunnerPlugin *plugin = m_plugins.at( current.row() );
    QWidget* configWidget = m_configWidgets[plugin];
    if ( configWidget ) {
        m_ui->settingsStack->setCurrentWidget( configWidget );
        QStandardItem *item = m_servicesModel->invisibleRootItem()->child( current.row() );
        m_ui->settingsStack->setEnabled( item->checkState() == Qt::Checked );
    } else {
        m_ui->settingsStack->setEnabled( false );
        m_ui->settingsStack->setCurrentWidget( m_ui->noConfigAvailablePage );
    }
}

void RoutingProfileSettingsDialog::editProfile( int profileIndex )
{
    QList<RoutingProfile> profiles = m_profilesModel->profiles();
    m_ui->name->setText( profiles.at( profileIndex ).name() );

    m_servicesModel->clear();
    foreach( RunnerPlugin *plugin,  m_plugins ) {
        QStandardItem *item = new QStandardItem( plugin->name() );
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

    if ( exec() != QDialog::Accepted) return;

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


}

#include "RoutingProfileSettingsDialog.moc"
