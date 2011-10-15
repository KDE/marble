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


#include "PlacemarkInfoDialog.h"

#include <cmath>

#include <QtCore/QFile>
#include <QtCore/QLocale>
#include <QtCore/QString>
#include <QtCore/QTimer>
#include <QtGui/QFont>
#include <QtGui/QStatusBar>

#include "DeferredFlag.h"
#include "GeoDataCoordinates.h"
#include "GeoDataExtendedData.h"
#include "MarbleClock.h"
#include "MarbleDirs.h"

using namespace Marble;
/* TRANSLATOR Marble::PlacemarkInfoDialog */

PlacemarkInfoDialog::PlacemarkInfoDialog(const GeoDataPlacemark *placemark, const MarbleClock *clock, QWidget *parent)
    : QDialog(parent),
      m_placemark(placemark),
      m_clock(clock)
{
    setupUi(this);

    setWindowTitle( tr("Marble Info Center - %1").arg( m_placemark->name() ) );
    resize( 780, 580 );

    m_pBackButton->hide();
    
    connect( m_pPrintButton, SIGNAL( clicked() ),
             m_pWikipediaBrowser, SLOT( print() ) );

    connect( m_pWikipediaBrowser, SIGNAL( statusMessage( QString ) ),
             this,                SLOT( showMessage( QString) ) );
    connect( this,                SIGNAL( source( QString ) ),
             m_pWikipediaBrowser, SLOT( setWikipediaPath( QString ) ) );//for wikipedia

    showContent();
}

void PlacemarkInfoDialog::showMessage( const QString& text )
{
    QFont statusFont = QStatusBar().font();
    statusFont.setPointSize( qRound( 0.9 * statusFont.pointSize() ) );
    m_pStatusLabel->setFont( statusFont );
    m_pStatusLabel->setText( text );
}

void PlacemarkInfoDialog::showContent()
{
    elevation_lbl->setVisible( true );
    elevation_val_lbl->setVisible( true );
    population_lbl->setText( tr("Population:") );


    name_val_lbl->setText( "<H1><b>" + m_placemark->name() + "</b></H1>" );
    altername_val_lbl->setText( "" );
    QString  rolestring;
    const QString role = m_placemark->role();
     if(role=="PPLC")
        rolestring = tr("National Capital");
    else if(role=="PPL")
        rolestring = tr("City");
    else if(role=="PPLA")
        rolestring = tr("State Capital");
    else if(role=="PPLA2")
        rolestring = tr("County Capital");
    else if(role=="PPLA3" || role=="PPLA4" )
        rolestring = tr("Capital");
    else if(role=="PPLF" || role=="PPLG" || role=="PPLL" || role=="PPLQ" || role=="PPLR" || role=="PPLS" || role=="PPLW" )
        rolestring = tr("Village");
    else if(role=="P" || role=="M" )
        rolestring = tr("Location");
    else if(role=="H")
    {    
        if ( m_placemark->popularity() > 0 )
            rolestring = tr("Mountain");
        else
            rolestring = tr("Elevation extreme");
    }
    else if(role=="V")
        rolestring = tr("Volcano");
    else if(role=="W")
        rolestring = tr("Shipwreck");
    else if(role=="O")
        rolestring = tr("Ocean");
    else if(role=="S")
        rolestring = tr("Nation");
    else if(role=="K")
        rolestring = tr("Continent");
    else if(role=="A")
        rolestring = tr("Astronomical observatory");
    else if(role=="a")
        rolestring = tr("Maria");
    else if(role=="c")
        rolestring = tr("Crater");
    else if(role=="h" || role=="r" || role=="u" || role=="i")
        rolestring = tr("Landing Site");
    else 
	rolestring = tr("Other Place");
	
    role_val_lbl->setText( rolestring );

    m_flagcreator = new DeferredFlag( this );
    requestFlag( m_placemark->countryCode() );


    const QString description = m_placemark->description();

    description_val_browser->setEnabled( false );
    if ( !description.isEmpty() )
    {
        description_val_browser->setEnabled( true );
        description_val_browser->setHtml( description );
    }

    GeoDataCoordinates coordinates = m_placemark->coordinate( m_clock->dateTime() );

    coordinates_val_lbl->setText( coordinates.toString() );
    country_val_lbl->setText( m_placemark->countryCode() );
    QString gmt = QString( "%1" ).arg( m_placemark->extendedData().value("gmt").value().toInt()/( double ) 100, 0, 'f', 1 );
    QString dst = QString( "%1" ).arg( ( m_placemark->extendedData().value("gmt").value().toInt() + m_placemark->extendedData().value("dst").value().toInt() )/( double ) 100, 0, 'f', 1 );
    gmtdst_val_lbl->setText( gmt + " / " + dst );
    state_val_lbl->setText( m_placemark->state() );

    const qint64 population = m_placemark->population();
    const qreal area = m_placemark->area();
    const qreal altitude = coordinates.altitude();

    area_lbl->setText( tr("Area:") );
    if ( area < 10000000 )
        area_val_lbl->setText( tr("%1 sq km").arg( QLocale::system().toString( area ) ) );
    else
        area_val_lbl->setText( tr("%1 Mio. sq km")
                .arg( QLocale::system().toString( area / 1000000.0, 'g', 4 ) ) );

    if ( population < 10000000 )
        population_val_lbl->setText( tr("%1 inh.").arg( QLocale::system().toString( population ) ) );
    else
        population_val_lbl->setText( tr("%1 Mio. inh.")
            .arg( QLocale::system().toString( population / 1000000.0, 'g', 4 ) ) );

    elevation_val_lbl->setText( tr("%1 m").arg( QLocale::system().toString( altitude ) ) );
    diameter_val_lbl->setText( tr("%1 km").arg( QLocale::system().toString( population / 1000.0, 'g', 4 ) ) );

    country_lbl->setVisible( true );
    country_val_lbl->setVisible( true );
    elevation_lbl->setVisible( true );
    elevation_val_lbl->setVisible( true );
    area_lbl->setVisible( true );
    area_val_lbl->setVisible( true );
    population_lbl->setVisible( true );
    population_val_lbl->setVisible( true );
    diameter_lbl->setVisible( false );
    diameter_val_lbl->setVisible( false );
    gmtdst_lbl->setVisible( true );
    gmtdst_val_lbl->setVisible( true );
    state_lbl->setVisible( true );
    state_val_lbl->setVisible( true );

    if ( altitude <= 0 )
        elevation_val_lbl->setText( tr("-") );

    if (    role == "O" ||  role == "o" || role == "v" || role == "h" || role == "u"
         || role == "i" || role == "r" || role == "a" || role == "c" || role == "m" ) {
        population_val_lbl->setVisible( false );
        population_lbl->setVisible( false );
        country_lbl->setVisible( false );
        country_val_lbl->setVisible( false );
        gmtdst_lbl->setVisible( false );
        gmtdst_val_lbl->setVisible( false );
        state_lbl->setVisible( false );
        state_val_lbl->setVisible( false );
    }

    if ( role == "A" ) {
        population_val_lbl->setVisible( false );
        population_lbl->setVisible( false );
        country_lbl->setVisible( false );
        country_val_lbl->setVisible( false );
        elevation_val_lbl->setVisible( false );
        elevation_lbl->setVisible( false );
        gmtdst_lbl->setVisible( false );
        gmtdst_val_lbl->setVisible( false );
        state_lbl->setVisible( false );
        state_val_lbl->setVisible( false );
    }

    if ( role == "K" )
    {
        country_lbl->setVisible( false );
        country_val_lbl->setVisible( false );
        gmtdst_lbl->setVisible( false );
        gmtdst_val_lbl->setVisible( false );
        state_lbl->setVisible( false );
        state_val_lbl->setVisible( false );
    }
    if ( (role == "a" || role == "c" || role == "m") && m_placemark->popularity() > 0) {
        diameter_lbl->setVisible( true );
        diameter_val_lbl->setVisible( true );
        gmtdst_lbl->setVisible( false );
        gmtdst_val_lbl->setVisible( false );
        state_lbl->setVisible( false );
        state_val_lbl->setVisible( false );
    }

    if ( role == "H" || role == "V" || role == "W") {
        population_val_lbl->setVisible( false );
        population_lbl->setVisible( false );
        area_val_lbl->setVisible( false );
        area_lbl->setVisible( false );
        gmtdst_lbl->setVisible( false );
        gmtdst_val_lbl->setVisible( false );
        state_lbl->setVisible( false );
        state_val_lbl->setVisible( false );
    }
    else if ( role == "P" || role == "M" ) {
        population_val_lbl->setVisible( false );
        population_lbl->setVisible( false );
        elevation_val_lbl->setVisible( false );
        elevation_lbl->setVisible( false );
        area_val_lbl->setVisible( false );
        area_lbl->setVisible( false );
        gmtdst_lbl->setVisible( false );
        gmtdst_val_lbl->setVisible( false );
        state_lbl->setVisible( false );
        state_val_lbl->setVisible( false );
    }
    else if ( role == "O" || role == "K" || role == "S"  )
    {
        elevation_lbl->setVisible( false );
        elevation_val_lbl->setVisible( false );
        gmtdst_lbl->setVisible( false );
        gmtdst_val_lbl->setVisible( false );
        state_lbl->setVisible( false );
        state_val_lbl->setVisible( false );
    }
    else{
        area_val_lbl->setVisible( false );
        area_lbl->setVisible( false );
    }

    emit source( QString("wiki/%1").arg( m_placemark->name() ) );
}


void PlacemarkInfoDialog::requestFlag( const QString& countrycode )
{
    QString filename = MarbleDirs::path( QString("flags/flag_%1.svg").arg( countrycode.toLower() ) );
    m_flagcreator->setFlag( filename, flag_val_lbl->size() );

    if ( QFile::exists( filename ) ) {
        connect( m_flagcreator, SIGNAL( flagDone() ), 
                 this,          SLOT( setFlagLabel() ) );
        QTimer::singleShot(100, m_flagcreator, SLOT(slotDrawFlag()));	
    }
} 


void PlacemarkInfoDialog::setFlagLabel()
{
    flag_val_lbl->setPixmap( m_flagcreator->flag() );
}

#include "PlacemarkInfoDialog.moc"
