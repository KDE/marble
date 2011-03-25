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

#include "EditBookmarkDialog.h"
#include "BookmarkManager.h"
#include "GeoDataPlacemark.h"
#include "GeoDataPoint.h"
#include "GeoDataFolder.h"
#include "GeoDataCoordinates.h"
#include "GeoDataExtendedData.h"
#include "MarbleModel.h"
#include "NewBookmarkFolderDialog.h"

#include <QtCore/QPointer>

namespace Marble {

class EditBookmarkDialogPrivate {
public:
    MarbleWidget *m_widget;
    MarbleRunnerManager* m_manager;
    BookmarkManager* m_bookmarkManager;
    GeoDataLookAt m_bookmarkLookAt;

    EditBookmarkDialogPrivate( EditBookmarkDialog* q, BookmarkManager *bookmarkManager );

    void initComboBox();

    void initialize();

    void setFolderName( const QString &name );

private:
    EditBookmarkDialog* const q;
};

EditBookmarkDialogPrivate::EditBookmarkDialogPrivate( EditBookmarkDialog* q_, BookmarkManager *bookmarkManager ) :
        m_widget( 0 ), m_manager( 0 ), m_bookmarkManager( bookmarkManager ), q( q_ )
{
    // nothing to do
}

void EditBookmarkDialogPrivate::initialize()
{
    q->setupUi( q );
    q->m_newFolderButton->setVisible( false );
    QObject::connect( q, SIGNAL( accepted() ), q, SLOT( addBookmark() ) );
    QObject::connect( q->m_newFolderButton, SIGNAL( clicked() ), q, SLOT( openNewFolderDialog() ) );

    initComboBox();
}

void EditBookmarkDialogPrivate::initComboBox()
{
    q->m_folders->clear();
    QVector<GeoDataFolder*> folders =  m_bookmarkManager->folders();
    QVector<GeoDataFolder*>::const_iterator i = folders.constBegin();
    QVector<GeoDataFolder*>::const_iterator end = folders.constEnd();

    QList<QString> folderNames;
    for (; i != end; ++i ) {
        folderNames.append( (*i)->name() );
    }

    q->m_folders->insertItems( 0, folderNames );
}

void EditBookmarkDialogPrivate::setFolderName( const QString &name )
{
    for ( int i=0; i<q->m_folders->count(); ++i ) {
        if ( q->m_folders->itemText( i ) == name ) {
            q->m_folders->setCurrentIndex( i );
            return;
        }
    }
}

EditBookmarkDialog::EditBookmarkDialog( BookmarkManager *bookmarkManager, QWidget *parent )
    : QDialog( parent ), d( new EditBookmarkDialogPrivate( this, bookmarkManager ) )
{
    d->initialize();
}

void EditBookmarkDialog::setLookAt( const GeoDataLookAt &lookAt )
{
    d->m_bookmarkLookAt = lookAt;
    if ( m_name->text().isEmpty() ) {
        m_name->setText( lookAt.coordinates().toString() );
        m_name->selectAll();
    }
}

void EditBookmarkDialog::setName( const QString &text )
{
    m_name->setText( text );
}

void EditBookmarkDialog::setDescription( const QString &text )
{
    m_description->setText( text );
}

void EditBookmarkDialog::setFolderName( const QString &name )
{
    d->setFolderName( name );
}

void EditBookmarkDialog::setMarbleWidget( MarbleWidget* widget )
{
    d->m_widget = widget;
    m_newFolderButton->setVisible( true );

    //reverse geocode the bookmark point for better user experience
    d->m_manager = new MarbleRunnerManager( d->m_widget->model()->pluginManager(), this );
    d->m_manager->setModel( d->m_widget->model() );
    QObject::connect( d->m_manager, SIGNAL( reverseGeocodingFinished( GeoDataCoordinates, GeoDataPlacemark ) ),
            this, SLOT( retrieveGeocodeResult( GeoDataCoordinates, GeoDataPlacemark ) ) );
    d->m_manager->reverseGeocoding( d->m_bookmarkLookAt.coordinates() );
}

EditBookmarkDialog::~EditBookmarkDialog()
{
    delete d;
}

void EditBookmarkDialog::retrieveGeocodeResult( const GeoDataCoordinates &coordinates, const GeoDataPlacemark &placemark)
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

    m_name->setText( bookmarkName );
    m_name->selectAll();
}

QString EditBookmarkDialog::append( const QString &bookmark, const QString &text)
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

void EditBookmarkDialog::openNewFolderDialog()
{
    QPointer<NewBookmarkFolderDialog> dialog = new NewBookmarkFolderDialog( this );
    if ( d->m_widget ) {
        dialog->setBookmarkManager( d->m_widget->model()->bookmarkManager() );
    }
    if ( dialog->exec() == QDialog::Accepted ) {
        d->initComboBox();
        d->setFolderName( dialog->folderName() );
    }
    delete dialog;
}

void EditBookmarkDialog::addBookmark()
{
    if ( d->m_widget ) {
        d->m_widget->model()->bookmarkManager()->addBookmark( bookmark(), folderName() );
    }
}

GeoDataPlacemark EditBookmarkDialog::bookmark() const
{
    //Create a bookmark object
    GeoDataPlacemark bookmark;
    bookmark.setName( name() );
    bookmark.setDescription( description() );
    //allow for HTML in the description
    bookmark.setDescriptionCDATA( true );
    bookmark.setCoordinate( d->m_bookmarkLookAt.coordinates() );

    bookmark.extendedData().addValue( GeoDataData( "isBookmark", true ) );
    bookmark.setLookAt( new GeoDataLookAt( d->m_bookmarkLookAt ) );

    return bookmark;
}

QString EditBookmarkDialog::name() const
{
    return m_name->text();
}

QString EditBookmarkDialog::folderName() const
{
    return m_folders->currentText();
}

QString EditBookmarkDialog::description() const
{
    return m_description->toPlainText();
}

}

#include "EditBookmarkDialog.moc"
