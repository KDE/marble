//
// This file is part of the Marble Desktop Globe.
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
#include "MarbleDirs.h"
#include "MarblePlacemarkModel.h"

using namespace Marble;

PlacemarkInfoDialog::PlacemarkInfoDialog(const QPersistentModelIndex &index, QWidget *parent)
    : QDialog(parent), m_index(index)
{
    setupUi(this);

    setWindowTitle( tr("Marble Info Center - %1").arg( m_index.data().toString() ) );
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


    name_val_lbl->setText( "<H1><b>" + m_index.data().toString() + "</b></H1>" );
    altername_val_lbl->setText( "" );
    QString  rolestring;
    const QString role = m_index.data( MarblePlacemarkModel::GeoTypeRole ).toString();
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
        if ( m_index.data( MarblePlacemarkModel::PopularityRole ).toInt() > 0 )
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
    requestFlag( m_index.data( MarblePlacemarkModel::CountryCodeRole ).toString() );


    const QString description = m_index.data( MarblePlacemarkModel::DescriptionRole ).toString();

    description_val_browser->setEnabled( false );
    if ( !description.isEmpty() )
    {
        description_val_browser->setEnabled( true );
        description_val_browser->setHtml( description );
    }
    coordinates_val_lbl->setText( m_index.data( MarblePlacemarkModel::CoordinateRole ).value<GeoDataCoordinates>().toString() );
    country_val_lbl->setText( m_index.data( MarblePlacemarkModel::CountryCodeRole ).toString() );

    const qint64 population = m_index.data( MarblePlacemarkModel::PopulationRole ).toLongLong();
    const qreal area = m_index.data( MarblePlacemarkModel::AreaRole ).toDouble();
    const qreal altitude = m_index.data( MarblePlacemarkModel::CoordinateRole ).value<GeoDataCoordinates>().altitude();

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
    timezone_lbl->setVisible( false );
    timezone_val_lbl->setVisible( false );

    if ( altitude <= 0 )
        elevation_val_lbl->setText( tr("-") );

    if (    role == "O" ||  role == "o" || role == "v" || role == "h" || role == "u"
         || role == "i" || role == "r" || role == "a" || role == "c" || role == "m" ) {
        population_val_lbl->setVisible( false );
        population_lbl->setVisible( false );
        country_lbl->setVisible( false );
        country_val_lbl->setVisible( false );
    }

    if ( role == "A" ) {
        population_val_lbl->setVisible( false );
        population_lbl->setVisible( false );
        country_lbl->setVisible( false );
        country_val_lbl->setVisible( false );
        elevation_val_lbl->setVisible( false );
        elevation_lbl->setVisible( false );
    }

    if ( role == "K" )
    {
        country_lbl->setVisible( false );
        country_val_lbl->setVisible( false );
    }
    if ( (role == "a" || role == "c" || role == "m") && m_index.data( MarblePlacemarkModel::PopularityRole ).toInt() > 0) {
        diameter_lbl->setVisible( true );
        diameter_val_lbl->setVisible( true );
    }

    if ( role == "H" || role == "V" || role == "W") {
        population_val_lbl->setVisible( false );
        population_lbl->setVisible( false );
        area_val_lbl->setVisible( false );
        area_lbl->setVisible( false );
    }
    else if ( role == "P" || role == "M" ) {
        population_val_lbl->setVisible( false );
        population_lbl->setVisible( false );
        elevation_val_lbl->setVisible( false );
        elevation_lbl->setVisible( false );
        area_val_lbl->setVisible( false );
        area_lbl->setVisible( false );
    }
    else if ( role == "O" || role == "K" || role == "S"  )
    {
        elevation_lbl->setVisible( false );
        elevation_val_lbl->setVisible( false );
    }
    else{
        area_val_lbl->setVisible( false );
        area_lbl->setVisible( false );
    }

    emit source( QString("wiki/%1").arg( m_index.data().toString() ) );
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
