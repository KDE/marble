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
#include "GeoDataLookAt.h"
#include "GeoDataDocument.h"
#include "GeoDataPlacemark.h"
#include "GeoDataPoint.h"
#include "GeoDataStyle.h"
#include "GeoDataIconStyle.h"
#include "GeoDataFolder.h"
#include "GeoDataCoordinates.h"
#include "GeoDataExtendedData.h"
#include "GeoDataData.h"
#include "MarbleDirs.h"
#include "MarbleModel.h"
#include "MarbleWidget.h"
#include "NewBookmarkFolderDialog.h"
#include "ReverseGeocodingRunnerManager.h"

#include <QPointer>

namespace Marble {

class EditBookmarkDialogPrivate {
public:
    MarbleWidget *m_widget;
    ReverseGeocodingRunnerManager* m_manager;
    BookmarkManager* m_bookmarkManager;
    GeoDataCoordinates m_bookmarkCoordinates;
    qreal m_range;
    Ui::UiEditBookmarkDialog m_ui;

    EditBookmarkDialogPrivate( EditBookmarkDialog* q, BookmarkManager *bookmarkManager );

    void initComboBox( const GeoDataContainer* const container );

    void initialize();

    void setFolderName( const QString &name );

    static QString append( const QString &bookmark, const QString &text);

    void openNewFolderDialog();

    void retrieveGeocodeResult( const GeoDataCoordinates &coordinates, const GeoDataPlacemark &placemark);

    void updateCoordinates();

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
    bool const smallScreen = MarbleGlobal::getInstance()->profiles() & MarbleGlobal::SmallScreen;
    m_ui.m_header->setPositionVisible(!smallScreen);
    m_ui.m_header->setIdVisible( false );
    m_ui.m_header->setIconLink(MarbleDirs::path(QStringLiteral("bitmaps/bookmark.png")));
    QObject::connect( m_ui.m_newFolderButton, SIGNAL(clicked()), q, SLOT(openNewFolderDialog()) );
    QObject::connect( m_ui.m_header, SIGNAL(valueChanged()), q, SLOT(updateCoordinates()) );

    m_ui.m_folders->clear();
    initComboBox( m_bookmarkManager->document() );
}

void EditBookmarkDialogPrivate::initComboBox( const GeoDataContainer* const container )
{
    m_bookmarkManager->ensureDefaultFolder();
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
    QObject::disconnect( d->m_ui.m_header, SIGNAL(valueChanged()), this, SLOT(updateCoordinates()) );

    if ( d->m_ui.m_header->name().isEmpty() ) {
        d->m_ui.m_header->setName( coordinates.toString() );
    }

    d->m_ui.m_header->setLongitude( coordinates.longitude(GeoDataCoordinates::Degree) );
    d->m_ui.m_header->setLatitude( coordinates.latitude(GeoDataCoordinates::Degree) );
    QObject::connect( d->m_ui.m_header, SIGNAL(valueChanged()), this, SLOT(updateCoordinates()) );
}

void EditBookmarkDialog::setRange( qreal range ) {
    d->m_range = range;
}

void EditBookmarkDialog::setName( const QString &text )
{
    QObject::disconnect( d->m_ui.m_header, SIGNAL(valueChanged()), this, SLOT(updateCoordinates()) );
    d->m_ui.m_header->setName( text );
    d->m_ui.m_header->selectAll();
    QObject::connect( d->m_ui.m_header, SIGNAL(valueChanged()), this, SLOT(updateCoordinates()) );
}

void EditBookmarkDialog::setDescription( const QString &text )
{
    QObject::disconnect( d->m_ui.m_header, SIGNAL(valueChanged()), this, SLOT(updateCoordinates()) );
    d->m_ui.m_description->setText( text );
    QObject::connect( d->m_ui.m_header, SIGNAL(valueChanged()), this, SLOT(updateCoordinates()) );
}

void EditBookmarkDialog::setFolderName( const QString &name )
{
    d->setFolderName( name );
}

void EditBookmarkDialog::setIconLink(const QString &iconLink)
{
    QObject::disconnect( d->m_ui.m_header, SIGNAL(valueChanged()), this, SLOT(updateCoordinates()) );
    d->m_ui.m_header->setIconLink( iconLink );
    QObject::connect( d->m_ui.m_header, SIGNAL(valueChanged()), this, SLOT(updateCoordinates()) );
}

void EditBookmarkDialog::setMarbleWidget( MarbleWidget* widget )
{
    d->m_widget = widget;
    const AngleUnit defaultAngleUnit = d->m_widget->defaultAngleUnit();
    const GeoDataCoordinates::Notation notation =
        (defaultAngleUnit == DecimalDegree) ? GeoDataCoordinates::Decimal :
        (defaultAngleUnit == DMSDegree) ?     GeoDataCoordinates::DMS :
        /* else, UTM */                       GeoDataCoordinates::UTM;
    d->m_ui.m_header->setNotation(notation);

    d->m_manager = new ReverseGeocodingRunnerManager( d->m_widget->model(), this );
    QObject::connect( d->m_manager, SIGNAL(reverseGeocodingFinished(GeoDataCoordinates,GeoDataPlacemark)),
            this, SLOT(retrieveGeocodeResult(GeoDataCoordinates,GeoDataPlacemark)) );
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

void EditBookmarkDialogPrivate::retrieveGeocodeResult( const GeoDataCoordinates &coordinates, const GeoDataPlacemark &placemark)
{
    Q_UNUSED(coordinates)
    GeoDataExtendedData data = placemark.extendedData();
    QString bookmarkName;
    qreal distance = m_widget->distance() * KM2METER;
    //FIXME : Optimal logic for suggestion with distance consideration is required

    if( distance >= 3500 ) {
        bookmarkName = data.value(QStringLiteral("country")).value().toString();
    }
    else if( distance >= 200 ) {
        bookmarkName = append(data.value(QStringLiteral("city")).value().toString(),
                              data.value(QStringLiteral("state")).value().toString());
        bookmarkName = append(bookmarkName, data.value(QStringLiteral("country")).value().toString());
    }
    else {
        bookmarkName = append(data.value(QStringLiteral("road")).value().toString(),
                              data.value(QStringLiteral("city")).value().toString());
        bookmarkName = append(bookmarkName, data.value(QStringLiteral("country")).value().toString());
    }

    if( bookmarkName.isEmpty() ) {
        bookmarkName = placemark.address();
    }

    m_ui.m_header->setName( bookmarkName );
    m_ui.m_header->selectAll();
}

QString EditBookmarkDialogPrivate::append( const QString &bookmark, const QString &text)
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

void EditBookmarkDialogPrivate::openNewFolderDialog()
{
    QPointer<NewBookmarkFolderDialog> dialog = new NewBookmarkFolderDialog( q );
    if ( dialog->exec() == QDialog::Accepted ) {
        m_bookmarkManager->addNewBookmarkFolder( m_bookmarkManager->document(), dialog->folderName() );
        m_ui.m_folders->clear();
        initComboBox( m_bookmarkManager->document() );
        setFolderName( dialog->folderName() );
    }
    delete dialog;
}

void EditBookmarkDialogPrivate::updateCoordinates()
{
    m_bookmarkCoordinates.setLongitude( m_ui.m_header->longitude(), GeoDataCoordinates::Degree );
    m_bookmarkCoordinates.setLatitude( m_ui.m_header->latitude(), GeoDataCoordinates::Degree );
}

GeoDataPlacemark EditBookmarkDialog::bookmark() const
{
    //Create a bookmark object
    GeoDataPlacemark bookmark;
    bookmark.setName( name() );
    bookmark.setDescription( description() );
    GeoDataStyle::Ptr newStyle(new GeoDataStyle( *bookmark.style() ));
    newStyle->iconStyle().setIconPath( iconLink() );
    bookmark.setStyle( newStyle );
    //allow for HTML in the description
    bookmark.setDescriptionCDATA( true );
    bookmark.setCoordinate( coordinates() );
    if ( d->m_range ) {
        GeoDataLookAt *lookat = new GeoDataLookAt;
        lookat->setCoordinates( coordinates() );
        lookat->setRange( range() );
        bookmark.setAbstractView( lookat );
    }

    bookmark.extendedData().addValue(GeoDataData(QStringLiteral("isBookmark"), true));

    if(d->m_widget != 0) {
        const QString celestialName = d->m_widget->model()->planetId();
        if (celestialName != QLatin1String("earth")) {
            bookmark.extendedData().addValue(GeoDataData(QStringLiteral("celestialBody"), celestialName));
        }
    }

    return bookmark;
}

QString EditBookmarkDialog::name() const
{
    return d->m_ui.m_header->name();
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

QString EditBookmarkDialog::iconLink() const
{
    return d->m_ui.m_header->iconLink();
}

}

#include "moc_EditBookmarkDialog.cpp"
