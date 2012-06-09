//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010 Gaurav Gupta <1989.gaurav@googlemail.com>
// Copyright 2011 Friedrich W. H. Kossebau <kossebau@kde.org>
// Copyright 2012 Thibaut Gridel <tgridel@free.fr>
//

#include "EditBookmarkDialog.h"
#include "ui_EditBookmarkDialog.h"

#include "BookmarkManager.h"
#include "GeoDataPlacemark.h"
#include "GeoDataPoint.h"
#include "GeoDataFolder.h"
#include "GeoDataCoordinates.h"
#include "GeoDataExtendedData.h"
#include "MarbleModel.h"
#include "MarbleRunnerManager.h"
#include "MarbleWidget.h"
#include "NewBookmarkFolderDialog.h"

#include <QtCore/QPointer>

namespace Marble {

class EditBookmarkDialogPrivate {
public:
    MarbleWidget *m_widget;
    MarbleRunnerManager* m_manager;
    BookmarkManager* m_bookmarkManager;
    GeoDataCoordinates m_bookmarkCoordinates;
    qreal m_range;
    Ui::UiEditBookmarkDialog m_ui;

    EditBookmarkDialogPrivate( EditBookmarkDialog* q, BookmarkManager *bookmarkManager );

    void initComboBox( const GeoDataContainer* const container );

    void initialize();

    void setFolderName( const QString &name );

private:
    EditBookmarkDialog* const q;
};

EditBookmarkDialogPrivate::EditBookmarkDialogPrivate( EditBookmarkDialog* q_, BookmarkManager *bookmarkManager ) :
        m_widget( 0 ), m_manager( 0 ), m_bookmarkManager( bookmarkManager ), m_range( 0 ), q( q_ )
{
    // nothing to do
}

void EditBookmarkDialogPrivate::initialize()
{
    m_ui.setupUi( q );
    m_ui.m_longitude->setDimension( Longitude );
    m_ui.m_latitude->setDimension( Latitude );
    QObject::connect( m_ui.m_newFolderButton, SIGNAL( clicked() ), q, SLOT( openNewFolderDialog() ) );
    QObject::connect( m_ui.m_longitude, SIGNAL( valueChanged(qreal) ), q, SLOT( onCoordinatesEdited() ) );
    QObject::connect( m_ui.m_latitude, SIGNAL( valueChanged(qreal) ), q, SLOT( onCoordinatesEdited() ) );

    m_ui.m_folders->clear();
    initComboBox( m_bookmarkManager->document() );
}

void EditBookmarkDialogPrivate::initComboBox( const GeoDataContainer* const container )
{
    foreach( GeoDataFolder *folder, container->folderList() ) {
        QVariant folderVariant;
        folderVariant.setValue(folder);
        m_ui.m_folders->addItem( folder->name(), folderVariant );
        if( !folder->folderList().isEmpty() ) {
            initComboBox( folder );
        }
    }
}

void EditBookmarkDialogPrivate::setFolderName( const QString &name )
{
    for ( int i=0; i<m_ui.m_folders->count(); ++i ) {
        if ( m_ui.m_folders->itemText( i ) == name ) {
            m_ui.m_folders->setCurrentIndex( i );
            return;
        }
    }
}

EditBookmarkDialog::EditBookmarkDialog( BookmarkManager *bookmarkManager, QWidget *parent )
    : QDialog( parent ), d( new EditBookmarkDialogPrivate( this, bookmarkManager ) )
{
    d->initialize();
}

void EditBookmarkDialog::setCoordinates( const GeoDataCoordinates &coordinates )
{
    d->m_bookmarkCoordinates = coordinates;

    if ( d->m_ui.m_name->text().isEmpty() ) {
        d->m_ui.m_name->setText( coordinates.toString() );
        d->m_ui.m_name->selectAll();
    }

    d->m_ui.m_longitude->setValue( coordinates.longitude(GeoDataCoordinates::Degree) );
    d->m_ui.m_latitude->setValue( coordinates.latitude(GeoDataCoordinates::Degree) );
}

void EditBookmarkDialog::setRange( qreal range ) {
    d->m_range = range;
}

void EditBookmarkDialog::setName( const QString &text )
{
    d->m_ui.m_name->setText( text );
}

void EditBookmarkDialog::setDescription( const QString &text )
{
    d->m_ui.m_description->setText( text );
}

void EditBookmarkDialog::setFolderName( const QString &name )
{
    d->setFolderName( name );
}

void EditBookmarkDialog::setMarbleWidget( MarbleWidget* widget )
{
    d->m_widget = widget;
    const AngleUnit defaultAngleUnit = d->m_widget->defaultAngleUnit();
    const LatLonEdit::Notation notation =
        (defaultAngleUnit == DecimalDegree) ? LatLonEdit::Decimal :
        (defaultAngleUnit == DMSDegree) ?     LatLonEdit::DMS :
        /* else, UTM */                       LatLonEdit::DMS;
    d->m_ui.m_longitude->setNotation( notation );
    d->m_ui.m_latitude->setNotation( notation );

    d->m_manager = new MarbleRunnerManager( d->m_widget->model()->pluginManager(), this );
    d->m_manager->setModel( d->m_widget->model() );
    QObject::connect( d->m_manager, SIGNAL( reverseGeocodingFinished( GeoDataCoordinates, GeoDataPlacemark ) ),
            this, SLOT( retrieveGeocodeResult( GeoDataCoordinates, GeoDataPlacemark ) ) );
}

void EditBookmarkDialog::setReverseGeocodeName()
{
    if ( !d->m_manager ) {
        return;
    }
    //reverse geocode the bookmark point for better user experience
    d->m_manager->reverseGeocoding( d->m_bookmarkCoordinates );
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

    d->m_ui.m_name->setText( bookmarkName );
    d->m_ui.m_name->selectAll();
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
    if ( dialog->exec() == QDialog::Accepted ) {
        d->m_bookmarkManager->addNewBookmarkFolder( folder(), dialog->folderName() );
        d->initComboBox( d->m_bookmarkManager->document() );
        d->setFolderName( dialog->folderName() );
    }
    delete dialog;
}

void EditBookmarkDialog::onCoordinatesEdited()
{
    d->m_bookmarkCoordinates.setLongitude( d->m_ui.m_longitude->value(), GeoDataCoordinates::Degree );
    d->m_bookmarkCoordinates.setLatitude( d->m_ui.m_latitude->value(), GeoDataCoordinates::Degree );
}

GeoDataPlacemark EditBookmarkDialog::bookmark() const
{
    //Create a bookmark object
    GeoDataPlacemark bookmark;
    bookmark.setName( name() );
    bookmark.setDescription( description() );
    //allow for HTML in the description
    bookmark.setDescriptionCDATA( true );
    bookmark.setCoordinate( coordinates() );
    if ( d->m_range ) {
        GeoDataLookAt *lookat = new GeoDataLookAt;
        lookat->setCoordinates( coordinates() );
        lookat->setRange( range() );
        bookmark.setLookAt( lookat );
    }

    bookmark.extendedData().addValue( GeoDataData( "isBookmark", true ) );

    return bookmark;
}

QString EditBookmarkDialog::name() const
{
    return d->m_ui.m_name->text();
}

GeoDataFolder *EditBookmarkDialog::folder() const
{
    return qvariant_cast<GeoDataFolder*>(d->m_ui.m_folders->itemData(d->m_ui.m_folders->currentIndex()));
}

QString EditBookmarkDialog::description() const
{
    return d->m_ui.m_description->toPlainText();
}

GeoDataCoordinates EditBookmarkDialog::coordinates() const
{
    return d->m_bookmarkCoordinates;
}

qreal EditBookmarkDialog::range() const {
    return d->m_range;
}

}

#include "EditBookmarkDialog.moc"
