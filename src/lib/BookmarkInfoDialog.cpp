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
#include "NewFolderInfoDialog.h"
#include <QLineEdit>
#include <QString>
#include <QPointer>

using namespace Marble;

BookmarkInfoDialog::BookmarkInfoDialog(MarbleWidget *parent)
    : QDialog(parent), m_widget(parent)
{
    setupUi(this);
    setWindowTitle( tr("Add Bookmark") );
    connect( m_saveButton, SIGNAL( clicked() ), this, SLOT( addBookmark() ) );
    connect( m_newFolderButton, SIGNAL( clicked() ), this, SLOT( openNewFolderDialog() ) );
        
    //Initializing ComboBox
    initComboBox();
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
