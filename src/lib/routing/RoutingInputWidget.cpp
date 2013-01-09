//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010      Dennis Nienhüser  <earthwings@gentoo.org>
// Copyright 2012      Illya Kovalevskyy <illya.kovalevskyy@gmail.com>
//

#include "RoutingInputWidget.h"

#include "MarbleLocale.h"
#include "MarblePlacemarkModel.h"
#include "MarbleRunnerManager.h"
#include "RouteRequest.h"
#include "TinyWebBrowser.h"
#include "BookmarkManager.h"
#include "MarbleModel.h"
#include "MarbleWidget.h"
#include "routing/RoutingManager.h"
#include "GeoDataFolder.h"
#include "PositionTracking.h"
#include "MarbleLineEdit.h"
#include "GoToDialog.h"

#include <QtCore/QTimer>
#include <QtCore/QUrl>
#include <QtGui/QHBoxLayout>
#include <QtGui/QIcon>
#include <QtGui/QPushButton>
#include <QtGui/QMenu>
#include <QtGui/QToolButton>
#include <QtGui/QKeyEvent>
#include <QtXml/QDomDocument>

namespace Marble
{

/**
  * A MarbleLineEdit that swallows enter/return pressed
  * key events
  */
class RoutingInputLineEdit : public MarbleLineEdit
{
public:
    RoutingInputLineEdit( QWidget *parent = 0 );

protected:
    virtual void keyPressEvent(QKeyEvent *);
};

class RoutingInputWidgetPrivate
{
public:
    MarbleModel* m_marbleModel;

    MarbleWidget* m_marbleWidget;

    RoutingInputLineEdit *m_lineEdit;

    MarbleRunnerManager m_runnerManager;

    MarblePlacemarkModel *m_placemarkModel;

    RouteRequest *m_route;

    int m_index;

    QTimer m_nominatimTimer;

    QVector<QIcon> m_progressAnimation;

    int m_currentFrame;

    QAction* m_bookmarkAction;

    QAction* m_mapInput;

    QAction* m_currentLocationAction;

    QAction* m_centerAction;

    QMenu *m_menu;

    /** Constructor */
    RoutingInputWidgetPrivate( MarbleWidget* widget, int index, QWidget *parent );

    /** Initiate reverse geocoding request to download address */
    void adjustText();

    void createMenu( RoutingInputWidget *parent );

    QMenu* createBookmarkMenu( RoutingInputWidget *parent );

    void createBookmarkActions( QMenu* menu, GeoDataFolder* bookmarksFolder, QObject *parent );

    QPixmap addDropDownIndicator( const QPixmap &pixmap ) const;
};

RoutingInputLineEdit::RoutingInputLineEdit( QWidget *parent ) :
    MarbleLineEdit( parent )
{
#if QT_VERSION >= 0x40700
    setPlaceholderText( QObject::tr( "Address or search term..." ) );
#endif
}

void RoutingInputLineEdit::keyPressEvent(QKeyEvent *event)
{
    MarbleLineEdit::keyPressEvent( event );
    bool const returnPressed = event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter;
    if ( returnPressed ) {
        event->accept();
    }
}

RoutingInputWidgetPrivate::RoutingInputWidgetPrivate( MarbleWidget* widget, int index, QWidget *parent ) :
        m_marbleModel( widget->model() ), m_marbleWidget( widget ), m_lineEdit( 0 ),
        m_runnerManager( m_marbleModel->pluginManager() ),
        m_placemarkModel( 0 ), m_route( m_marbleModel->routingManager()->routeRequest() ), m_index( index ),
        m_currentFrame( 0 ),
        m_bookmarkAction( 0 ), m_mapInput( 0 ), m_currentLocationAction( 0 ),
        m_centerAction( 0 )
{
    m_runnerManager.setModel( m_marbleModel );

    m_lineEdit = new RoutingInputLineEdit( parent );
    m_lineEdit->setDecorator( addDropDownIndicator( m_route->pixmap( m_index ) ) );

    m_nominatimTimer.setInterval( 1000 );
    m_nominatimTimer.setSingleShot( true );
}

void RoutingInputWidgetPrivate::adjustText()
{
    m_nominatimTimer.start();
}

void RoutingInputWidgetPrivate::createMenu( RoutingInputWidget *parent )
{
    QMenu* result = new QMenu( parent );

    m_centerAction = result->addAction( QIcon( m_route->pixmap( m_index ) ), QObject::tr( "&Center Map here" ),
                       parent, SLOT( requestActivity() ) );
    result->addSeparator();

    m_currentLocationAction = result->addAction( QIcon( ":/icons/gps.png" ), QObject::tr( "Current &Location" ),
                                                 parent, SLOT( setCurrentLocation() ) );
    m_currentLocationAction->setEnabled( false );

    m_mapInput = result->addAction( QIcon( ":/icons/crosshairs.png" ), QObject::tr( "From &Map..." ) );
    m_mapInput->setCheckable( true );
    QObject::connect( m_mapInput, SIGNAL( triggered( bool ) ), parent, SLOT( setMapInputModeEnabled( bool ) ) );

    m_bookmarkAction = result->addAction( QIcon( ":/icons/bookmarks.png" ), QObject::tr( "From &Bookmark" ) );
    m_bookmarkAction->setMenu( createBookmarkMenu( parent ) );

    m_menu = result;
}

QMenu* RoutingInputWidgetPrivate::createBookmarkMenu( RoutingInputWidget *parent )
{
    QMenu* result = new QMenu( parent );
    result->addAction( QIcon( ":/icons/go-home.png" ), QObject::tr( "&Home" ), parent, SLOT( setHomePosition() ) );

    QVector<GeoDataFolder*> folders = m_marbleModel->bookmarkManager()->folders();

    if ( folders.size() == 1 ) {
        createBookmarkActions( result, folders.first(), parent );
    } else {
        QVector<GeoDataFolder*>::const_iterator i = folders.constBegin();
        QVector<GeoDataFolder*>::const_iterator end = folders.constEnd();

        for (; i != end; ++i ) {
            QMenu* menu = result->addMenu( QIcon( ":/icons/folder-bookmark.png" ), (*i)->name() );
            createBookmarkActions( menu, *i, parent );
        }
    }

    return result;
}

void RoutingInputWidgetPrivate::createBookmarkActions( QMenu* menu, GeoDataFolder* bookmarksFolder, QObject *parent )
{
    QVector<GeoDataPlacemark*> bookmarks = bookmarksFolder->placemarkList();
    QVector<GeoDataPlacemark*>::const_iterator i = bookmarks.constBegin();
    QVector<GeoDataPlacemark*>::const_iterator end = bookmarks.constEnd();

    for (; i != end; ++i ) {
        QAction *bookmarkAction = new QAction( (*i)->name(), parent );
        bookmarkAction->setData( qVariantFromValue( (*i)->coordinate() ) );
        menu->addAction( bookmarkAction );
        QObject::connect( menu, SIGNAL( triggered( QAction* ) ), parent, SLOT( setBookmarkPosition( QAction* ) ) );
    }
}

QPixmap RoutingInputWidgetPrivate::addDropDownIndicator(const QPixmap &pixmap) const
{
    QPixmap result( pixmap.size() + QSize( 8, pixmap.height() ) );
    result.fill( QColor( Qt::transparent ) );
    QPainter painter( &result );
    painter.drawPixmap( 0, 0, pixmap );
    QPoint const one( pixmap.width() + 1, pixmap.height() - 8 );
    QPoint const two( one.x() + 6, one.y() );
    QPoint const three( one.x() + 3, one.y() + 4 );
    painter.setRenderHint( QPainter::Antialiasing, true );
    painter.setPen( Qt::NoPen );
    painter.setBrush( QColor( Oxygen::aluminumGray4 ) );
    painter.drawConvexPolygon( QPolygon() << one << two << three );
    return result;
}

RoutingInputWidget::RoutingInputWidget( MarbleWidget* widget, int index, QWidget *parent ) :
        QWidget( parent ), d( new RoutingInputWidgetPrivate( widget, index, this ) )
{
    connect(d->m_lineEdit, SIGNAL(decoratorButtonClicked()), this, SLOT(showMenu()));
    QHBoxLayout *layout = new QHBoxLayout( this );
    layout->setSpacing( 0 );
    layout->setMargin( 0 );

    bool const smallScreen = MarbleGlobal::getInstance()->profiles() & MarbleGlobal::SmallScreen;
    if ( smallScreen ) {
        layout->addWidget( d->m_lineEdit );
        connect( d->m_lineEdit, SIGNAL( decoratorButtonClicked() ), this, SLOT( openTargetSelectionDialog() ) );
    } else {
        d->createMenu( this );
        layout->addWidget( d->m_lineEdit );
    }

    connect( d->m_lineEdit, SIGNAL( clearButtonClicked() ), this, SLOT( requestRemoval() ) );
    connect( d->m_marbleModel->bookmarkManager(), SIGNAL( bookmarksChanged() ),
             this, SLOT( reloadBookmarks() ) );
    connect( d->m_marbleModel->positionTracking(), SIGNAL( statusChanged( PositionProviderStatus ) ),
             this, SLOT( updateCurrentLocationButton( PositionProviderStatus ) ) );
    connect( &d->m_runnerManager, SIGNAL( searchResultChanged( QAbstractItemModel * ) ),
             this, SLOT( setPlacemarkModel( QAbstractItemModel * ) ) );
    connect( &d->m_runnerManager, SIGNAL( reverseGeocodingFinished( GeoDataCoordinates, GeoDataPlacemark )),
             this, SLOT(retrieveReverseGeocodingResult( GeoDataCoordinates, GeoDataPlacemark ) ) );
    connect( d->m_lineEdit, SIGNAL( returnPressed() ),
             this, SLOT( findPlacemarks() ) );
    connect( d->m_lineEdit, SIGNAL( textEdited( QString ) ),
             this, SLOT( setInvalid() ) );
    connect( &d->m_runnerManager, SIGNAL( searchFinished( QString ) ),
             this, SLOT( finishSearch() ) );
    connect( d->m_marbleModel->routingManager()->routeRequest(), SIGNAL( positionChanged( int, GeoDataCoordinates ) ),
             this, SLOT( updatePosition( int, GeoDataCoordinates ) ) );
    connect( &d->m_nominatimTimer, SIGNAL( timeout() ),
             this, SLOT( reverseGeocoding() ) );
    connect( this, SIGNAL( targetValidityChanged( bool ) ), this, SLOT( updateCenterButton( bool ) ) );
    updateCenterButton( hasTargetPosition() );

    d->adjustText();
}

RoutingInputWidget::~RoutingInputWidget()
{
    delete d;
}

void RoutingInputWidget::reverseGeocoding()
{
    if ( !hasTargetPosition() ) {
        return;
    }

    QString const name = d->m_route->name( d->m_index );
    if ( name.isEmpty() || name == tr( "Current Location" ) ) {
        d->m_runnerManager.reverseGeocoding( targetPosition() );
    } else {
        d->m_lineEdit->setText( name );
        d->m_lineEdit->setCursorPosition( 0 );
    }
}

void RoutingInputWidget::setPlacemarkModel( QAbstractItemModel *model )
{
    d->m_placemarkModel = dynamic_cast<MarblePlacemarkModel*>(model);
}

void RoutingInputWidget::setTargetPosition( const GeoDataCoordinates &position, const QString &name )
{
    if ( d->m_mapInput ) {
        d->m_mapInput->setChecked( false );
    }
    d->m_route->setPosition( d->m_index, position, name );
    if ( !name.isEmpty() ) {
        d->m_lineEdit->setText( name );
        d->m_lineEdit->setCursorPosition( 0 );
    }
    emit targetValidityChanged( true );
}

bool RoutingInputWidget::hasTargetPosition() const
{
    GeoDataCoordinates pos = targetPosition();
    return pos.longitude() != 0.0 && pos.latitude() != 0.0;
}

GeoDataCoordinates RoutingInputWidget::targetPosition() const
{
    if ( d->m_index < d->m_route->size() ) {
        return d->m_route->at( d->m_index );
    } else {
        return GeoDataCoordinates();
    }
}

void RoutingInputWidget::findPlacemarks()
{
    QString text = d->m_lineEdit->text();
    if ( text.isEmpty() ) {
        setInvalid();
    } else {
        d->m_lineEdit->setBusy(true);
        d->m_runnerManager.findPlacemarks( text );
    }
}

MarblePlacemarkModel *RoutingInputWidget::searchResultModel()
{
    return d->m_placemarkModel;
}

void RoutingInputWidget::requestActivity()
{
    if ( hasTargetPosition() ) {
        emit activityRequest( this );
    }
}

void RoutingInputWidget::requestRemoval()
{
    emit removalRequest( this );
}

bool RoutingInputWidget::hasInput() const
{
    return !d->m_lineEdit->text().isEmpty();
}

void RoutingInputWidget::setMapInputModeEnabled( bool enabled )
{
    emit mapInputModeEnabled( this, enabled );
}

void RoutingInputWidget::finishSearch()
{
    d->m_lineEdit->setBusy(false);
    emit searchFinished( this );
}

void RoutingInputWidget::setInvalid()
{
    d->m_route->setPosition( d->m_index, GeoDataCoordinates() );
    emit targetValidityChanged( false );
}

void RoutingInputWidget::abortMapInputRequest()
{
    if ( d->m_mapInput ) {
        d->m_mapInput->setChecked( false );
    }
}

void RoutingInputWidget::setIndex( int index )
{
    d->m_index = index;
    d->m_lineEdit->setBusy(false);
    d->m_lineEdit->setDecorator( d->addDropDownIndicator( d->m_route->pixmap( index ) ) );
}

void RoutingInputWidget::updatePosition( int index, const GeoDataCoordinates & )
{
    if ( index == d->m_index ) {
        d->m_lineEdit->setBusy(false);
        emit targetValidityChanged( hasTargetPosition() );
        d->adjustText();
    }
}

void RoutingInputWidget::clear()
{
    d->m_nominatimTimer.stop();
    d->m_lineEdit->setBusy(false);
    d->m_route->setPosition( d->m_index, GeoDataCoordinates() );
    d->m_lineEdit->clear();
    emit targetValidityChanged( false );
}

void RoutingInputWidget::retrieveReverseGeocodingResult( const GeoDataCoordinates &coordinates, const GeoDataPlacemark &placemark )
{
    QString description = placemark.address().isEmpty() ? coordinates.toString().trimmed() : placemark.address();
    d->m_route->setName( d->m_index, description );
    d->m_lineEdit->setText( description );
    d->m_lineEdit->setCursorPosition( 0 );
}

void RoutingInputWidget::setProgressAnimation( const QVector<QIcon> &animation )
{
    d->m_progressAnimation = animation;
}

void RoutingInputWidget::reloadBookmarks()
{
    if ( d->m_bookmarkAction ) {
        d->m_bookmarkAction->setMenu( d->createBookmarkMenu( this ) );
    }
}

void RoutingInputWidget::setHomePosition()
{
    qreal lon( 0.0 ), lat( 0.0 );
    int zoom( 0 );
    d->m_marbleModel->home( lon, lat, zoom );
    GeoDataCoordinates home( lon, lat, 0.0, GeoDataCoordinates::Degree );
    setTargetPosition( home );
    requestActivity();
}

void RoutingInputWidget::updateCurrentLocationButton( PositionProviderStatus status )
{
    if ( d->m_currentLocationAction ) {
        d->m_currentLocationAction->setEnabled( status == PositionProviderStatusAvailable );
    }
}

void RoutingInputWidget::setCurrentLocation()
{
    setTargetPosition( d->m_marbleModel->positionTracking()->currentLocation() );
    requestActivity();
}

void RoutingInputWidget::updateCenterButton( bool hasPosition )
{
    if ( d->m_centerAction ) {
        d->m_centerAction->setEnabled( hasPosition );
    }
}

void RoutingInputWidget::setBookmarkPosition( QAction* bookmark )
{
    if ( !bookmark->data().isNull() ) {
        setTargetPosition( qVariantValue<GeoDataCoordinates>( bookmark->data() ) );
        requestActivity();
    }
}

void RoutingInputWidget::openTargetSelectionDialog()
{
    QPointer<GoToDialog> dialog = new GoToDialog( d->m_marbleModel, this );
    dialog->setWindowTitle( tr( "Choose Placemark" ) );
    dialog->setShowRoutingItems( false );
    dialog->setSearchEnabled( false );
    if ( dialog->exec() == QDialog::Accepted ) {
        const GeoDataCoordinates coordinates = dialog->coordinates();
        setTargetPosition( coordinates );
    }
    delete dialog;
}

void RoutingInputWidget::showMenu()
{
    d->m_menu->exec( mapToGlobal( QPoint( 0, size().height() ) ) );
}

} // namespace Marble

#include "RoutingInputWidget.moc"
