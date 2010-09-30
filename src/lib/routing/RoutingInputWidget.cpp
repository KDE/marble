//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010      Dennis Nienhüser <earthwings@gentoo.org>
//

#include "RoutingInputWidget.h"

#include "MarbleLocale.h"
#include "MarblePlacemarkModel.h"
#include "MarbleRunnerManager.h"
#include "RouteRequest.h"
#include "TinyWebBrowser.h"
#include "BookmarkManager.h"
#include "MarbleModel.h"
#include "MarbleMap.h"
#include "routing/RoutingManager.h"
#include "GeoDataFolder.h"
#include "PositionTracking.h"
#include "RoutingLineEdit.h"

#include <QtCore/QTimer>
#include <QtCore/QUrl>
#include <QtGui/QHBoxLayout>
#include <QtGui/QIcon>
#include <QtGui/QPushButton>
#include <QtGui/QMenu>
#include <QtGui/QToolButton>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QtXml/QDomDocument>

namespace Marble
{

class RoutingInputWidgetPrivate
{
public:
    MarbleModel* m_marbleModel;

    MarbleMap* m_marbleMap;

    RoutingLineEdit *m_lineEdit;

    QToolButton *m_menuButton;

    MarbleRunnerManager *m_runnerManager;

    MarblePlacemarkModel *m_placemarkModel;

    QTimer m_progressTimer;

    RouteRequest *m_route;

    int m_index;

    QNetworkAccessManager *m_manager;

    QTimer m_nominatimTimer;

    QVector<QIcon> m_progressAnimation;

    int m_currentFrame;

    QAction* m_bookmarkAction;

    QAction* m_mapInput;

    QAction* m_currentLocationAction;

    QAction* m_centerAction;

    /** Constructor */
    RoutingInputWidgetPrivate( MarbleMap* map, int index, QWidget *parent );

    /** Initiate reverse geocoding request to download address */
    void adjustText();

    QMenu* createMenu( RoutingInputWidget *parent );

    QMenu* createBookmarkMenu( RoutingInputWidget *parent );

    void createBookmarkActions( QMenu* menu, GeoDataFolder* bookmarksFolder, QObject *parent );
};

RoutingInputWidgetPrivate::RoutingInputWidgetPrivate( MarbleMap* map, int index, QWidget *parent ) :
        m_marbleModel( map->model() ), m_marbleMap( map ), m_lineEdit( 0 ),
        m_runnerManager( new MarbleRunnerManager( m_marbleModel->pluginManager(), parent ) ),
        m_placemarkModel( 0 ), m_route( m_marbleModel->routingManager()->routeRequest() ), m_index( index ),
        m_manager( new QNetworkAccessManager( parent ) ), m_currentFrame( 0 ),
        m_bookmarkAction( 0 ), m_mapInput( 0 ), m_currentLocationAction( 0 ),
        m_centerAction( 0 )
{
    bool smallScreen = MarbleGlobal::getInstance()->profiles() & MarbleGlobal::SmallScreen;
    int const iconSize = smallScreen ? 32 : 16;

    m_menuButton = new QToolButton( parent );
    m_menuButton->setIcon( QIcon( m_route->pixmap( m_index ) ) );
    m_menuButton->setPopupMode( QToolButton::InstantPopup );
    m_menuButton->setIconSize( QSize( iconSize, iconSize ) );

    m_lineEdit = new RoutingLineEdit( parent );

    m_progressTimer.setInterval( 100 );
    m_nominatimTimer.setInterval( 1000 );
    m_nominatimTimer.setSingleShot( true );
}

void RoutingInputWidgetPrivate::adjustText()
{
    m_nominatimTimer.start();
}

QMenu* RoutingInputWidgetPrivate::createMenu( RoutingInputWidget *parent )
{
    QMenu* result = new QMenu( parent );

    m_centerAction = result->addAction( QIcon( m_route->pixmap( m_index ) ), QObject::tr( "&Center Map here" ),
                       parent, SLOT( requestActivity() ) );
    result->addSeparator();

    m_currentLocationAction = result->addAction( QIcon( ":/icons/gps.png" ), "Current &Location",
                                                 parent, SLOT( setCurrentLocation() ) );
    m_currentLocationAction->setEnabled( false );

    m_mapInput = result->addAction( QIcon( ":/icons/crosshairs.png" ), QObject::tr( "From &Map..." ) );
    m_mapInput->setCheckable( true );
    QObject::connect( m_mapInput, SIGNAL( triggered( bool ) ), parent, SLOT( setMapInputModeEnabled( bool ) ) );

    m_bookmarkAction = result->addAction( QIcon( ":/icons/bookmarks.png" ), QObject::tr( "From &Bookmark" ) );
    m_bookmarkAction->setMenu( createBookmarkMenu( parent ) );
    QObject::connect( m_lineEdit, SIGNAL( clearButtonClicked() ), parent, SLOT( requestRemoval() ) );
    return result;
}

QMenu* RoutingInputWidgetPrivate::createBookmarkMenu( RoutingInputWidget *parent )
{
    QMenu* result = new QMenu( parent );
    result->addAction( QIcon( ":/icons/go-home.png" ), "&Home", parent, SLOT( setHomePosition() ) );

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

RoutingInputWidget::RoutingInputWidget( MarbleMap* map, int index, QWidget *parent ) :
        QWidget( parent ), d( new RoutingInputWidgetPrivate( map, index, this ) )
{
    QHBoxLayout *layout = new QHBoxLayout( this );
    layout->setSpacing( 0 );
    layout->setMargin( 0 );

    layout->addWidget( d->m_menuButton );
    layout->addWidget( d->m_lineEdit );
    d->m_menuButton->setMenu( d->createMenu( this ) );

    connect( d->m_marbleModel->bookmarkManager(), SIGNAL( bookmarksChanged() ),
             this, SLOT( reloadBookmarks() ) );
    connect( d->m_marbleModel->positionTracking(), SIGNAL( statusChanged( PositionProviderStatus ) ),
             this, SLOT( updateCurrentLocationButton( PositionProviderStatus ) ) );
    connect( d->m_runnerManager, SIGNAL( searchResultChanged( MarblePlacemarkModel * ) ),
             this, SLOT( setPlacemarkModel( MarblePlacemarkModel * ) ) );
    connect( d->m_runnerManager, SIGNAL( reverseGeocodingFinished( GeoDataCoordinates, GeoDataPlacemark )),
             this, SLOT(retrieveReverseGeocodingResult( GeoDataCoordinates, GeoDataPlacemark ) ) );
    connect( d->m_lineEdit, SIGNAL( returnPressed() ),
             this, SLOT( findPlacemarks() ) );
    connect( d->m_lineEdit, SIGNAL( textEdited( QString ) ),
             this, SLOT( setInvalid() ) );
    connect( &d->m_progressTimer, SIGNAL( timeout() ),
             this, SLOT( updateProgress() ) );
    connect( d->m_runnerManager, SIGNAL( searchFinished( QString ) ),
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

    d->m_runnerManager->reverseGeocoding( targetPosition() );
}

void RoutingInputWidget::setPlacemarkModel( MarblePlacemarkModel *model )
{
    d->m_placemarkModel = model;
}

void RoutingInputWidget::setTargetPosition( const GeoDataCoordinates &position )
{
    d->m_route->setPosition( d->m_index, position );
    d->m_progressTimer.stop();
    emit targetValidityChanged( true );
}

bool RoutingInputWidget::hasTargetPosition() const
{
    GeoDataCoordinates pos = targetPosition();
    return pos.longitude() != 0.0 && pos.latitude() != 0.0;
}

GeoDataCoordinates RoutingInputWidget::targetPosition() const
{
    return d->m_route->at( d->m_index );
}

void RoutingInputWidget::findPlacemarks()
{
    QString text = d->m_lineEdit->text();
    if ( text.isEmpty() ) {
        setInvalid();
    } else {
        d->m_progressTimer.start();
        d->m_runnerManager->findPlacemarks( text );
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

void RoutingInputWidget::updateProgress()
{
    if ( !d->m_progressAnimation.isEmpty() ) {
        d->m_currentFrame = ( d->m_currentFrame + 1 ) % d->m_progressAnimation.size();
        QIcon frame = d->m_progressAnimation[d->m_currentFrame];
        d->m_menuButton->setIcon( frame );
    }
}

void RoutingInputWidget::finishSearch()
{
    d->m_progressTimer.stop();
    d->m_menuButton->setIcon( QIcon( d->m_route->pixmap( d->m_index ) ) );
    emit searchFinished( this );
}

void RoutingInputWidget::setInvalid()
{
    d->m_route->setPosition( d->m_index, GeoDataCoordinates() );
    emit targetValidityChanged( false );
}

void RoutingInputWidget::abortMapInputRequest()
{
    d->m_mapInput->setChecked( false );
}

void RoutingInputWidget::setIndex( int index )
{
    d->m_index = index;
    d->m_menuButton->setIcon( QIcon( d->m_route->pixmap( d->m_index ) ) );
}

void RoutingInputWidget::updatePosition( int index, const GeoDataCoordinates & )
{
    if ( index == d->m_index ) {
        d->m_menuButton->setIcon( d->m_route->pixmap( d->m_index ) );
        emit targetValidityChanged( hasTargetPosition() );
        d->adjustText();
    }
}

void RoutingInputWidget::setWorkOffline( bool offline )
{
    d->m_runnerManager->setWorkOffline( offline );
}

void RoutingInputWidget::clear()
{
    d->m_nominatimTimer.stop();
    d->m_progressTimer.stop();
    d->m_menuButton->setIcon( d->m_route->pixmap( d->m_index ) );
    d->m_route->setPosition( d->m_index, GeoDataCoordinates() );
    d->m_lineEdit->clear();
    emit targetValidityChanged( false );
}

void RoutingInputWidget::retrieveReverseGeocodingResult( const GeoDataCoordinates &, const GeoDataPlacemark &placemark )
{
    d->m_route->setName( d->m_index, placemark.address() );
    d->m_lineEdit->setText( placemark.address() );
    d->m_lineEdit->setCursorPosition( 0 );
}

void RoutingInputWidget::setProgressAnimation( const QVector<QIcon> &animation )
{
    d->m_progressAnimation = animation;
}

void RoutingInputWidget::reloadBookmarks()
{
    d->m_bookmarkAction->setMenu( d->createBookmarkMenu( this ) );
}

void RoutingInputWidget::setHomePosition()
{
    qreal lon( 0.0 ), lat( 0.0 );
    int zoom( 0 );
    d->m_marbleMap->home( lon, lat, zoom );
    GeoDataCoordinates home( lon, lat, 0.0, GeoDataCoordinates::Degree );
    setTargetPosition( home );
    requestActivity();
}

void RoutingInputWidget::updateCurrentLocationButton( PositionProviderStatus status )
{
    d->m_currentLocationAction->setEnabled( status == PositionProviderStatusAvailable );
}

void RoutingInputWidget::setCurrentLocation()
{
    setTargetPosition( d->m_marbleModel->positionTracking()->currentLocation() );
    requestActivity();
}

void RoutingInputWidget::updateCenterButton( bool hasPosition )
{
    d->m_centerAction->setEnabled( hasPosition );
}

void RoutingInputWidget::setBookmarkPosition( QAction* bookmark )
{
    if ( !bookmark->data().isNull() ) {
        setTargetPosition( qVariantValue<GeoDataCoordinates>( bookmark->data() ) );
        requestActivity();
    }
}

} // namespace Marble

#include "RoutingInputWidget.moc"
