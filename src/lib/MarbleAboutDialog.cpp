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
#include <QtGui/QTextFrame>
#include <QtCore/QTextStream>
#include <QtGui/QPixmap>

#include "global.h"
#include "MarbleDirs.h"


class MarbleAboutDialogPrivate
{
public: 
    Ui::MarbleAboutDialog  uiWidget;

    bool authorsLoaded;
    bool dataLoaded;
    bool licenseLoaded;
};


MarbleAboutDialog::MarbleAboutDialog(QWidget *parent)
    : QDialog( parent ),
      d( new MarbleAboutDialogPrivate )
{
    d->uiWidget.setupUi( this );

    d->authorsLoaded = false;
    d->dataLoaded = false;
    d->licenseLoaded = false;

    d->uiWidget.m_pMarbleLogoLabel->setPixmap( QPixmap( MarbleDirs::path("svg/marble-logo-72dpi.png") ) );
    d->uiWidget.m_pMarbleVersionLabel->setText( "Version " + MARBLE_VERSION_STRING );

    connect( d->uiWidget.tabWidget, SIGNAL( currentChanged( int ) ), 
             this, SLOT( loadPageContents( int ) ) );

    QTextBrowser* browser = d->uiWidget.m_pMarbleAboutBrowser;
    browser->setHtml( tr("<br />(c) 2007, The Marble Project<br /><br /><a href=\"http://edu.kde.org/marble\">http://edu.kde.org/marble</a>") );

    QTextFrameFormat  format = browser->document()->rootFrame()->frameFormat();
    format.setMargin(12);
    browser->document()->rootFrame()->setFrameFormat( format );

}

void MarbleAboutDialog::loadPageContents( int idx )
{
    QTextBrowser* browser = 0;

    if ( idx == 1 && d->authorsLoaded == false )
    {
        d->authorsLoaded = true;
        browser = d->uiWidget.m_pMarbleAuthorsBrowser;
        QString filename = MarbleDirs::path( "credits_authors.html" );
        if( !filename.isEmpty() )
        {
            QFile  f( filename );
            if( f.open( QIODevice::ReadOnly ) ) 
            {
                QTextStream ts( &f );
                browser->setHtml( ts.readAll() );
            }
            f.close();
        }
        QTextFrameFormat  format = browser->document()->rootFrame()->frameFormat();
        format.setMargin(12);
        browser->document()->rootFrame()->setFrameFormat( format );
    }

    if ( idx == 2 && d->dataLoaded == false )
    {
        d->dataLoaded = true;
        browser = d->uiWidget.m_pMarbleDataBrowser;
        QString filename = MarbleDirs::path( "credits_data.html" );
        if( !filename.isEmpty() )
        {
            QFile  f( filename );
            if( f.open( QIODevice::ReadOnly ) ) 
            {
                QTextStream ts( &f );
                browser->setHtml( ts.readAll() );
            }
            f.close();
        }
        QTextFrameFormat  format = browser->document()->rootFrame()->frameFormat();
        format.setMargin(12);
        browser->document()->rootFrame()->setFrameFormat( format );
    }

    if ( idx == 3 && d->licenseLoaded == false )
    {
        d->licenseLoaded = true;
        browser = d->uiWidget.m_pMarbleLicenseBrowser;
        QString filename = MarbleDirs::path( "LICENSE.txt" );
        if( !filename.isEmpty() )
        {
            QFile  f( filename );
            if( f.open( QIODevice::ReadOnly ) ) 
            {
                QTextStream ts( &f );
                browser->setText( ts.readAll() );
            }
            f.close();
        }
        QTextFrameFormat  format = browser->document()->rootFrame()->frameFormat();
        format.setMargin(12);
        browser->document()->rootFrame()->setFrameFormat( format );
    }
}

#include "MarbleAboutDialog.moc"

