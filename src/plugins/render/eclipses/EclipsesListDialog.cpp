//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2012 Rene Kuettner <rene@bitkanal.net>
//

#include "EclipsesListDialog.h"

#include "MarbleModel.h"
#include "MarbleClock.h"
#include "MarbleDebug.h"

#include "EclipsesModel.h"

#include "ui_EclipsesListDialog.h"

namespace Marble {

EclipsesListDialog::EclipsesListDialog( const MarbleModel *model,
                                        QWidget *parent )
    : QDialog( parent ),
    m_marbleModel( model )
{
    initialize();
}

EclipsesListDialog::~EclipsesListDialog()
{
    delete m_eclModel;
}

void EclipsesListDialog::setYear( int year )
{
    if( !isVisible() ) {
        m_listWidget->spinBoxYear->setValue( year );
    }
}

int EclipsesListDialog::year() const
{
    return m_listWidget->spinBoxYear->value();
}

void EclipsesListDialog::accept()
{
}

void EclipsesListDialog::updateEclipsesListForYear( int year )
{
    Q_ASSERT( year >= 0 );
    m_eclModel->setYear( year );
}

void EclipsesListDialog::initialize()
{
    m_listWidget = new Ui::EclipsesListDialog();
    m_listWidget->setupUi( this );

    connect( m_listWidget->buttonShow, SIGNAL(clicked()),
             this, SIGNAL(accept()) );
    connect( m_listWidget->buttonClose, SIGNAL(clicked()),
             this, SLOT(reject()) );
    connect( m_listWidget->buttonSettings, SIGNAL(clicked()),
             SIGNAL(buttonSettingsClicked()) );
    connect( m_listWidget->spinBoxYear, SIGNAL(valueChanged(int)),
             this, SLOT(updateEclipsesListForYear(int)) );
    connect( m_listWidget->treeView, SIGNAL(itemSelectionChanged()),
             this, SLOT(updateListDialogButtons) );

    m_eclModel = new EclipsesModel( m_marbleModel );
    m_listWidget->treeView->setModel( m_eclModel );

    setYear( m_marbleModel->clock()->dateTime().date().year() );

    update();
}

} // namespace Marble

#include "EclipsesListDialog.moc"

