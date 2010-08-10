//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010 Gaurav Gupta <1989.gaurav@googlemail.com>
//
//

#include "BookmarkInfoDialog.h"
#include "MarbleDebug.h"
#include "BookmarkManager.h"
#include "GeoDataPlacemark.h"
#include "GeoDataFolder.h"
#include "GeoDataCoordinates.h"
#include "NewFolderInfoDialog.h"
#include "GeoDataExtendedData.h"
#include <QtGui/QLineEdit>
#include <QtCore/QString>
#include <QtCore/QPointer>
#include <QtCore/QDebug>
#include "MarbleModel.h"
using namespace Marble;

BookmarkInfoDialog::BookmarkInfoDialog(MarbleWidget *parent)
    : QDialog( parent ), 
      m_widget( parent ), 
      m_manager( 0 )
{
    setupUi(this);
    setWindowTitle( tr("Add Bookmark") );
    connect( m_saveButton, SIGNAL( clicked() ), this, SLOT( addBookmark() ) );
    connect( m_newFolderButton, SIGNAL( clicked() ), this, SLOT( openNewFolderDialog() ) );

    m_manager = new MarbleRunnerManager( m_widget->model()->pluginManager(), this );

    //reverse geocode the bookmark point for better user experience    
    connect( m_manager, SIGNAL( reverseGeocodingFinished( GeoDataCoordinates, GeoDataPlacemark ) ),
            this, SLOT( retrieveGeocodeResult( GeoDataCoordinates, GeoDataPlacemark ) ) );  

    GeoDataCoordinates coordinates( m_widget->centerLongitude(), m_widget->centerLatitude(), 0, GeoDataCoordinates::Degree, 0 ) ;
    m_manager->reverseGeocoding( coordinates );
    

     name->setText(coordinates.toString() );
     name->selectAll();
    //Initialzing ComboBox
    initComboBox();
}

BookmarkInfoDialog::~BookmarkInfoDialog()
{
    delete m_manager;
}

void BookmarkInfoDialog::retrieveGeocodeResult( const GeoDataCoordinates &coordinates, const GeoDataPlacemark &placemark)
{
    
    GeoDataExtendedData extended = placemark.extendedData();
    QString bookmarkName = "";
    qreal distance = m_widget->distance() * KM2METER;        
    //FIXME : Optimal logic for suggestion with distance consideration is required

    if( distance >= 3500 ){
        bookmarkName = extended.value("country").value().toString() ;
    }
    else if( distance >= 200 ){
        bookmarkName = append( extended.value("town").value().toString()
                , extended.value("state").value().toString() );
        bookmarkName = append( bookmarkName, extended.value("country").value().toString() ) ;
    }
    else{ 
        bookmarkName = append( extended.value("road").value().toString()
            , extended.value("town").value().toString());
        bookmarkName = append( bookmarkName, extended.value("country").value().toString() ) ;
    }

    if( bookmarkName.isEmpty() ){
        bookmarkName = placemark.address();
    }

  /*  if( !extended.value("country").value().toString().isEmpty() )
        bookmarkName = extended.value("country").value().toString() ;
    else if( !extended.value("state").value().toString().isEmpty() )
        bookmarkName = extended.value("state").value().toString() ;
    else if( !extended.value("county").value().toString().isEmpty() )
        bookmarkName = extended.value("county").value().toString() ;
    else if( !extended.value("village").value().toString().isEmpty() )
        bookmarkName = extended.value("village").value().toString() ;
    else if( !extended.value("suburb").value().toString().isEmpty() )
        bookmarkName = extended.value("suburb").value().toString() ;
    else if( !extended.value("road").value().toString().isEmpty() )
        bookmarkName = extended.value("road").value().toString() ;
    */
    name->setText( bookmarkName );
    
    name->selectAll();
}

QString BookmarkInfoDialog::append( const QString &bookmark, const QString &text)
{
    if( bookmark.isEmpty() && text.isEmpty() ){
        return "";
    }
    else if( bookmark.isEmpty() ){
        return text;
    }
    else if( text.isEmpty() ){
        return bookmark;
    }
    return bookmark + "," + text;
}

void BookmarkInfoDialog::initComboBox()
{
    m_folders->clear();
    QVector<GeoDataFolder*> folders =  m_widget->folders();
    QVector<GeoDataFolder*>::const_iterator i = folders.constBegin();
    QVector<GeoDataFolder*>::const_iterator end = folders.constEnd();

    QList<QString> folderNames;
    for (; i != end; ++i ) {
        folderNames.append( (*i)->name() );
    }

    m_folders->insertItems( 0, folderNames );
}

void BookmarkInfoDialog::openNewFolderDialog()
{
    QPointer<NewFolderInfoDialog> dialog = new NewFolderInfoDialog( m_widget );
    dialog->exec();
    delete dialog;
    initComboBox(); 
}

void BookmarkInfoDialog::addBookmark()
{

    mDebug() << "Adding Bookmark with "
             << " longitude : " <<  m_widget->centerLongitude()
             << " latitude : " <<  m_widget->centerLatitude()
             << " distance : " <<  m_widget->distance() * KM2METER;


    //Create a bookmark object 
    GeoDataPlacemark bookmark;
    bookmark.setName( name->text() );
    bookmark.setDescription( description->toPlainText() );
    //allow for HTML in the description
    bookmark.setDescriptionCDATA( true );
    GeoDataLookAt *lookAt = new GeoDataLookAt( m_widget->lookAt() ) ;
    bookmark.setLookAt( lookAt );


    mDebug()<<"Selected Folder for bookmark addition : "<<m_folders->currentText();
    
    m_widget->addBookmark( bookmark, m_folders->currentText() );
}


#include "BookmarkInfoDialog.moc"
