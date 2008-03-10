//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2006-2007 Torsten Rahn <tackat@kde.org>"
// Copyright 2007      Inge Wallin  <ingwa@kde.org>"
//


// Own
#include "TileCreatorDialog.h"

// Qt
#include <QtCore/QDebug>
#include <QtCore/QTimer>

// Marble
#include "TileCreator.h"


TileCreatorDialog::TileCreatorDialog(TileCreator *creator, QWidget *parent)
    : QDialog(parent), m_creator( creator )
{
    setupUi(this);

    connect( m_creator, SIGNAL( progress( int ) ),
             this, SLOT( setProgress( int ) ), Qt::QueuedConnection );

    // Start the creation process
    m_creator->start();
}

TileCreatorDialog::~TileCreatorDialog()
{
    disconnect( m_creator, SIGNAL( progress( int ) ),
                this, SLOT( setProgress( int ) ) );

    if ( m_creator->isRunning() )
        m_creator->cancelTileCreation();
    m_creator->wait();
    m_creator->deleteLater();
}

void TileCreatorDialog::setProgress( int progress )
{
    progressBar->setValue( progress );

    if ( progress == 100 )
	{
        QTimer::singleShot( 0, this, SLOT( accept() ) ); 
	}
}

void TileCreatorDialog::setSummary( const QString& name, 
                                    const QString& description )
{ 
    const QString summary = "<B>" + tr( name.toUtf8() ) + "</B><BR>" + tr( description.toUtf8() );
    descriptionLabel->setText( summary );
}

#include "TileCreatorDialog.moc"
