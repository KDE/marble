//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010      Niko Sams <niko.sams@gmail.com>
//

#include "RoutingProfilesWidget.h"
#include "ui_RoutingSettingsWidget.h"
#include "MarbleWidget.h"

#include <QStandardItemModel>
#include <QDialog>
#include "MarbleModel.h"
#include "PluginManager.h"
#include "RunnerPlugin.h"
#include "MarbleDebug.h"
#include "RoutingProfilesModel.h"
#include "RoutingManager.h"
#include "RoutingProfileSettingsDialog.h"

namespace Marble
{

RoutingProfilesWidget::RoutingProfilesWidget( MarbleModel *marbleModel )
    : QWidget( 0 ), m_marbleModel( marbleModel ),
      dummy( 0 )
{
    m_ui = new Ui_RoutingSettingsWidget;
    m_ui->setupUi( this );

    m_profilesModel= marbleModel->routingManager()->profilesModel();
    m_ui->profilesList->setModel( m_profilesModel );

    connect( m_ui->addButton, SIGNAL( clicked( bool ) ), SLOT( add() ) );
    connect( m_ui->removeButton, SIGNAL( clicked( bool ) ), SLOT( remove( ) ) );
    connect( m_ui->configureButton, SIGNAL( clicked( bool ) ), SLOT( configure() ) );
    connect( m_ui->moveUpButton, SIGNAL( clicked( bool ) ), SLOT( moveUp() ) );
    connect( m_ui->moveDownButton, SIGNAL( clicked( bool ) ), SLOT( moveDown() ) );
    connect( m_ui->profilesList->selectionModel(), SIGNAL( currentRowChanged(QModelIndex,QModelIndex) ), SLOT( updateButtons() ), Qt::QueuedConnection );
    connect( m_profilesModel, SIGNAL( layoutChanged() ), SLOT( updateButtons() ) );
    connect( m_ui->profilesList, SIGNAL( doubleClicked( QModelIndex ) ), SLOT( configure() ) );
}

RoutingProfilesWidget::~RoutingProfilesWidget()
{
    delete m_ui;
}

void RoutingProfilesWidget::add()
{
    m_profilesModel->addProfile( tr( "New Profile" ) );

    int profileIndex = m_profilesModel->rowCount() - 1;
    m_ui->profilesList->selectionModel()->select( m_profilesModel->index( profileIndex, 0 ), QItemSelectionModel::Clear | QItemSelectionModel::SelectCurrent );

    RoutingProfileSettingsDialog dialog( m_marbleModel->pluginManager(), m_profilesModel, this );
    dialog.editProfile( profileIndex );
}

void RoutingProfilesWidget::remove()
{
    if ( m_ui->profilesList->selectionModel()->selectedRows().isEmpty() ) {
        return;
    }
    m_profilesModel->removeRows( m_ui->profilesList->selectionModel()->selectedRows().first().row(), 1 );
}

void RoutingProfilesWidget::configure()
{
    if ( m_ui->profilesList->selectionModel()->selectedRows().isEmpty() ) {
        return;
    }

    int profileIndex = m_ui->profilesList->selectionModel()->selectedRows().first().row();

    RoutingProfileSettingsDialog dialog( m_marbleModel->pluginManager(), m_profilesModel, this );
    dialog.editProfile( profileIndex );
}

void RoutingProfilesWidget::moveUp()
{
    if ( m_ui->profilesList->selectionModel()->selectedRows().isEmpty() ) {
        return;
    }
    m_profilesModel->moveUp( m_ui->profilesList->selectionModel()->selectedRows().first().row() );
}

void RoutingProfilesWidget::moveDown()
{
    if ( m_ui->profilesList->selectionModel()->selectedRows().isEmpty() ) {
        return;
    }
    m_profilesModel->moveDown( m_ui->profilesList->selectionModel()->selectedRows().first().row() );
}

void RoutingProfilesWidget::updateButtons()
{
    QModelIndex current;
    if ( !m_ui->profilesList->selectionModel()->selectedRows().isEmpty() ) {
        current = m_ui->profilesList->selectionModel()->selectedRows().first();
    }
    m_ui->configureButton->setEnabled( current.isValid() );
    m_ui->removeButton->setEnabled( current.isValid() );
    m_ui->moveUpButton->setEnabled( current.isValid() && current.row() > 0 );
    m_ui->moveDownButton->setEnabled( current.isValid() && current.row()+1 < m_profilesModel->rowCount()  );
}

}

#include "RoutingProfilesWidget.moc"
