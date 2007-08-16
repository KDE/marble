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

#include "TileCreatorDialog.h"

#include <QtCore/QDebug>
#include <QtCore/QTimer>

#include "TileCreator.h"


TileCreatorDialog::TileCreatorDialog(QWidget *parent) : QDialog(parent) 
{
    setupUi(this);
}


void TileCreatorDialog::setProgress( int progress )
{
    progressBar->setValue( progress );

    if ( progress == 100 )
        accept(); 
}

void TileCreatorDialog::setSummary( const QString& name, 
                                    const QString& description )
{ 
    QString summary = "<B>" + tr( name.toUtf8() ) + "</B><BR>" + tr( description.toUtf8() );
    descriptionLabel->setText( summary );
}


#include "TileCreatorDialog.moc"
