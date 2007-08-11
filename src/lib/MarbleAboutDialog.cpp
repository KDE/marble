/* This file is part of the KDE project
 *
 * This file is part of the Marble Desktop Globe.
 *
 * Copyright 2004-2007 Torsten Rahn <tackat@kde.org>"
 * Copyright 2007      Inge Wallin  <ingwa@kde.org>"
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */


#include "MarbleAboutDialog.h"
#include "ui_MarbleAboutDialog.h"

#include <QtCore/QFile>
#include <QtCore/QTextStream>
#include <QtGui/QPixmap>

#include "MarbleDirs.h"


class MarbleAboutDialogPrivate
{
public: 
    Ui::MarbleAboutDialog  uiWidget;
};


MarbleAboutDialog::MarbleAboutDialog(QWidget *parent)
    : QDialog( parent ),
      d( new MarbleAboutDialogPrivate )
{
    d->uiWidget.setupUi( this );

    d->uiWidget.m_pMarbleLogoLabel->setPixmap( QPixmap( MarbleDirs::path("svg/marble-logo-72dpi.png") ) );
    d->uiWidget.m_pMarbleAboutBrowser->setHtml( tr("<br />(c) 2007, The Marble Project<br /><br /><a href=\"http://edu.kde.org/marble\">http://edu.kde.org/marble</a>") );
    QString filename = MarbleDirs::path( "LICENSE.txt" );
    if( !filename.isEmpty() ) {
        QFile  f( filename );
        if( f.open( QIODevice::ReadOnly ) ) {
            QTextStream ts( &f );
            d->uiWidget.m_pMarbleLicenseBrowser->setText( ts.readAll() );
        }
        f.close();
    }
}


#include "MarbleAboutDialog.moc"

