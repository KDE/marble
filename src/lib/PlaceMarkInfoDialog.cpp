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


#include "PlaceMarkInfoDialog.h"

#include <cmath>

#include <QtCore/QFile>
#include <QtCore/QLocale>
#include <QtCore/QTimer>
#include <QtGui/QPainter>
#include <QtGui/QLabel>
#include <QtGui/QTextBrowser>

#include "MarbleDirs.h"
#include "PlaceMark.h"


PlaceMarkInfoDialog::PlaceMarkInfoDialog(PlaceMark* mark, QWidget *parent)
    : QDialog(parent), m_mark(mark)
{
    setupUi(this);

    m_pBackButton->hide();
    m_pForwardButton->hide();

    connect( m_pPrintButton, SIGNAL( clicked() ),
             m_pWikipediaBrowser, SLOT( print() ) );

    setWindowTitle( tr("Marble Info Center - %1").arg( mark->name() ) );

#if 0
      QLayout  *layout = this->layout();
      m_pStatusBar = new QStatusBar( this );
      layout->addWidget( m_pStatusBar );
      layout->setSpacing( 2 );
      layout->setMargin( 2 );
#endif

    connect( m_pWikipediaBrowser, SIGNAL( statusMessage( QString ) ),
             this,                SLOT( showMessage( QString) ) );
    connect( this,                SIGNAL( source( QString ) ),
             m_pWikipediaBrowser, SLOT( setSource( QString ) ) );

    showContent();
}


void PlaceMarkInfoDialog::showContent()
{
    name_val_lbl->setText( "<H1><b>" + m_mark->name() + "</b></H1>" );
    altername_val_lbl->setText( "" );

    QString  rolestring;
    switch ( m_mark->role().toLatin1() ) {
    case 'C':
        rolestring = tr("Capital");
        break;
    case 'B':
        rolestring = tr("Capital");
        break;
    case 'R':
        rolestring = tr("Regional Capital");
        break;
    case 'P':
        rolestring = tr("Location");
        break;
    case 'M':
        rolestring = tr("Location");
        break;
    case 'H':
        if ( m_mark->population() > 0 )
            rolestring = tr("Mountain");
        else
            rolestring = tr("Elevation extreme");
        break;
    case 'V':
        rolestring = tr("Volcano");
        break;
    default:
    case 'N':
        rolestring = tr("City");
        break;
    }

    role_val_lbl->setText( rolestring );

    m_flagcreator = new DeferredFlag( this );
    requestFlag( m_mark->countryCode() );

    QString  description = m_mark->description();
    if ( !description.isEmpty() )
        description_val_browser->setPlainText( description );

    coordinates_val_lbl->setText( m_mark->coordinate().toString() );
    country_val_lbl->setText( m_mark->countryCode() );

    if ( m_mark->role() == 'H' || m_mark->role() == 'V' ) {
        population_val_lbl->setVisible( false );
        population_lbl->setVisible( false );

        elevation_val_lbl->setText( tr("%1 m").arg( QLocale::system().toString( m_mark->population() / 1000 ) ) );
    }
    else if (m_mark->role() == 'P' || m_mark->role() == 'M') {
        population_val_lbl->setVisible( false );
        population_lbl->setVisible( false );
        elevation_val_lbl->setVisible( false );
        elevation_lbl->setVisible( false );
    }
    else{
        population_val_lbl->setText( tr("%1 inh.").arg( QLocale::system().toString( m_mark->population() ) ) );
        elevation_val_lbl->setText( tr("-") );
    }

    emit source( QString("wiki/%1").arg( m_mark->name() ) );
}


void PlaceMarkInfoDialog::requestFlag( const QString& countrycode )
{
    QString filename = MarbleDirs::path( QString("flags/flag_%1.svg").arg( countrycode.toLower() ) );
    m_flagcreator->setFlag( filename, flag_val_lbl->size() );

    if ( QFile::exists( filename ) ) {
        connect( m_flagcreator, SIGNAL( flagDone() ), 
                 this,          SLOT( setFlagLabel() ) );
        QTimer::singleShot(100, m_flagcreator, SLOT(slotDrawFlag()));	
    }
} 


void PlaceMarkInfoDialog::setFlagLabel()
{
    flag_val_lbl->setPixmap( m_flagcreator->flag() );
}

#include "PlaceMarkInfoDialog.moc"
