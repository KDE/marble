//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010 Gaurav Gupta <1989.gaurav@googlemail.com>
//
//

#include "BookmarkInfoDialog.h"
#include "BookmarkManager.h"
#include "GeoDataPlacemark.h"
#include "GeoDataPoint.h"
#include "GeoDataFolder.h"
#include "GeoDataCoordinates.h"
#include "GeoDataExtendedData.h"
#include "MarbleModel.h"
#include "NewFolderInfoDialog.h"

#include <QtCore/QPointer>

namespace Marble {

class BookmarkInfoDialogPrivate {
public:
    MarbleWidget *m_widget;
    MarbleRunnerManager* m_manager;
    GeoDataCoordinates m_bookmarkCoordinate;

    BookmarkInfoDialogPrivate( BookmarkInfoDialog* q, MarbleWidget *parent );

    void initComboBox();

    void initialize( const GeoDataCoordinates &coordinates );

private:
    BookmarkInfoDialog* const q;
};

BookmarkInfoDialogPrivate::BookmarkInfoDialogPrivate( BookmarkInfoDialog* q_, MarbleWidget *parent ) :
        m_widget( parent ), m_manager( 0 ), q( q_ )
{
    // nothing to do
}

void BookmarkInfoDialogPrivate::initialize( const GeoDataCoordinates &coordinates )
{
    m_bookmarkCoordinate = coordinates;
    q->setupUi( q );
    q->setWindowTitle( QObject::tr("Add Bookmark") );
    QObject::connect( q->m_saveButton, SIGNAL( clicked() ), q, SLOT( addBookmark() ) );
    QObject::connect( q->m_newFolderButton, SIGNAL( clicked() ), q, SLOT( openNewFolderDialog() ) );

    //reverse geocode the bookmark point for better user experience
    m_manager = new MarbleRunnerManager( m_widget->model()->pluginManager(), q );
    m_manager->setModel( m_widget->model() );
    QObject::connect( m_manager, SIGNAL( reverseGeocodingFinished( GeoDataCoordinates, GeoDataPlacemark ) ),
            q, SLOT( retrieveGeocodeResult( GeoDataCoordinates, GeoDataPlacemark ) ) );
    m_manager->reverseGeocoding( coordinates );

    q->name->setText(coordinates.toString() );
    q->name->selectAll();
    initComboBox();
}

void BookmarkInfoDialogPrivate::initComboBox()
{
    q->m_folders->clear();
    QVector<GeoDataFolder*> folders =  m_widget->bookmarkFolders();
    QVector<GeoDataFolder*>::const_iterator i = folders.constBegin();
    QVector<GeoDataFolder*>::const_iterator end = folders.constEnd();

    QList<QString> folderNames;
    for (; i != end; ++i ) {
        folderNames.append( (*i)->name() );
    }

    q->m_folders->insertItems( 0, folderNames );
}

BookmarkInfoDialog::BookmarkInfoDialog(MarbleWidget *parent)
    : QDialog( parent ), d( new BookmarkInfoDialogPrivate( this, parent ) )
{
    GeoDataCoordinates coordinates( d->m_widget->centerLongitude(),
                                    d->m_widget->centerLatitude(), 0,
                                    GeoDataCoordinates::Degree, 0 ) ;
    d->initialize( coordinates );
}

BookmarkInfoDialog::BookmarkInfoDialog( const GeoDataCoordinates &coordinates, MarbleWidget *parent )
    : QDialog( parent ), d( new BookmarkInfoDialogPrivate( this, parent ) )
{
    d->initialize( coordinates );
}

BookmarkInfoDialog::~BookmarkInfoDialog()
{
    delete d;
}

void BookmarkInfoDialog::retrieveGeocodeResult( const GeoDataCoordinates &coordinates, const GeoDataPlacemark &placemark)
{
    Q_UNUSED(coordinates)
    GeoDataExtendedData data = placemark.extendedData();
    QString bookmarkName;
    qreal distance = d->m_widget->distance() * KM2METER;
    //FIXME : Optimal logic for suggestion with distance consideration is required

    if( distance >= 3500 ) {
        bookmarkName = data.value("country").value().toString() ;
    }
    else if( distance >= 200 ) {
        bookmarkName = append( data.value("city").value().toString()
                , data.value("state").value().toString() );
        bookmarkName = append( bookmarkName, data.value("country").value().toString() ) ;
    }
    else {
        bookmarkName = append( data.value("road").value().toString()
            , data.value("city").value().toString());
        bookmarkName = append( bookmarkName, data.value("country").value().toString() ) ;
    }

    if( bookmarkName.isEmpty() ) {
        bookmarkName = placemark.address();
    }

    name->setText( bookmarkName );
    name->selectAll();
}

QString BookmarkInfoDialog::append( const QString &bookmark, const QString &text)
{
    if( bookmark.isEmpty() && text.isEmpty() ) {
        return "";
    }
    else if( bookmark.isEmpty() ) {
        return text;
    }
    else if( text.isEmpty() ) {
        return bookmark;
    }
    return bookmark + ", " + text;
}

void BookmarkInfoDialog::openNewFolderDialog()
{
    QPointer<NewFolderInfoDialog> dialog = new NewFolderInfoDialog( d->m_widget );
    dialog->exec();
    delete dialog;
    d->initComboBox();
}

void BookmarkInfoDialog::addBookmark()
{
    //Create a bookmark object 
    GeoDataPlacemark bookmark;
    bookmark.setName( name->text() );
    bookmark.setDescription( description->toPlainText() );
    //allow for HTML in the description
    bookmark.setDescriptionCDATA( true );
    bookmark.setCoordinate( d->m_bookmarkCoordinate );

    bookmark.extendedData().addValue( GeoDataData( "isBookmark", true ) );
    GeoDataLookAt *lookAt = new GeoDataLookAt( d->m_widget->lookAt() ) ;
    lookAt->setLatitude( d->m_bookmarkCoordinate.latitude() );
    lookAt->setLongitude( d->m_bookmarkCoordinate.longitude() );
    bookmark.setLookAt( lookAt );

    d->m_widget->addBookmark( bookmark, m_folders->currentText() );
}

}

#include "BookmarkInfoDialog.moc"
