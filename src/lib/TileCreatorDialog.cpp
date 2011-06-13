//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2006-2007 Torsten Rahn <tackat@kde.org>
// Copyright 2007      Inge Wallin  <ingwa@kde.org>
//


// Own
#include "TileCreatorDialog.h"

// Qt
#include <QtCore/QTimer>

// Marble
#include "MarbleDebug.h"
#include "TileCreator.h"

#include "ui_TileCreatorDialog.h"

namespace Marble
{

class TileCreatorDialogPrivate
{
 public:
    Ui::TileCreatorDialog  uiWidget;

    TileCreator *m_creator;
};

    
TileCreatorDialog::TileCreatorDialog(TileCreator *creator, QWidget *parent)
    : QDialog(parent),
      d( new TileCreatorDialogPrivate )
{
    d->m_creator = creator;
    
    d->uiWidget.setupUi(this);

    connect( d->m_creator, SIGNAL( progress( int ) ),
             this, SLOT( setProgress( int ) ), Qt::QueuedConnection );
    connect( d->uiWidget.cancelButton, SIGNAL( clicked() ),
             this, SLOT( cancelTileCreation() ) );

    // Start the creation process
    d->m_creator->start();
}

void TileCreatorDialog::cancelTileCreation()
{
    d->uiWidget.cancelButton->setEnabled( false );

    /** @todo: Cancelling mostly crashes Marble. Fix that and uncomment below */
    // d->m_creator->cancelTileCreation();
}

TileCreatorDialog::~TileCreatorDialog()
{
    disconnect( d->m_creator, SIGNAL( progress( int ) ),
                this, SLOT( setProgress( int ) ) );

    if ( d->m_creator->isRunning() )
        d->m_creator->cancelTileCreation();
    d->m_creator->wait();
    d->m_creator->deleteLater();
    delete d;
}

void TileCreatorDialog::setProgress( int progress )
{
    d->uiWidget.progressBar->setValue( progress );

    if ( progress == 100 )
	{
        QTimer::singleShot( 0, this, SLOT( accept() ) ); 
	}
}

void TileCreatorDialog::setSummary( const QString& name, 
                                    const QString& description )
{ 
    const QString summary = "<B>" + tr( name.toUtf8() ) + "</B><BR>" + tr( description.toUtf8() );
    d->uiWidget.descriptionLabel->setText( summary );
}

}

#include "TileCreatorDialog.moc"
