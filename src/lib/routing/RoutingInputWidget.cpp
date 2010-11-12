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
#include "MarbleWidget.h"
#include "routing/RoutingManager.h"
#include "GeoDataFolder.h"
#include "PositionTracking.h"
#include "RoutingLineEdit.h"
#include "GoToDialog.h"

#include <QtCore/QTimer>
#include <QtCore/QUrl>
#include <QtGui/QHBoxLayout>
#include <QtGui/QIcon>
#include <QtGui/QPushButton>
#include <QtGui/QMenu>
#include <QtGui/QToolButton>
#include <QtGui/QKeyEvent>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QtXml/QDomDocument>

namespace Marble
{

/**
  * A RoutingLineEdit that swallows enter/return pressed
  * key events
  */
class RoutingInputLineEdit : public RoutingLineEdit
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
    RoutingInputWidgetPrivate( MarbleWidget* widget, int index, QWidget *parent );

    /** Initiate reverse geocoding request to download address */
    void adjustText();

    QMenu* createMenu( RoutingInputWidget *parent );

    QMenu* createBookmarkMenu( RoutingInputWidget *parent );

    void createBookmarkActions( QMenu* menu, GeoDataFolder* bookmarksFolder, QObject *parent );

    void setProgressAnimationEnabled( bool enabled );
};

RoutingInputLineEdit::RoutingInputLineEdit( QWidget *parent ) : RoutingLineEdit( parent )
{
#if QT_VERSION >= 0x40700
    setPlaceholderText( "Address or search term..." );
#endif
}

void RoutingInputLineEdit::keyPressEvent(QKeyEvent *event)
{
    RoutingLineEdit::keyPressEvent( event );
    bool const returnPressed = event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter;
    if ( returnPressed ) {
        event->accept();
    }
}

RoutingInputWidgetPrivate::RoutingInputWidgetPrivate( MarbleWidget* widget, int index, QWidget *parent ) :
        m_marbleModel( widget->model() ), m_marbleWidget( widget ), m_lineEdit( 0 ),
        m_runnerManager( new MarbleRunnerManager( m_marbleModel->pluginManager(), parent ) ),
        m_placemarkModel( 0 ), m_route( m_marbleModel->routingManager()->routeRequest() ), m_index( index ),
        m_manager( new QNetworkAccessManager( parent ) ), m_currentFrame( 0 ),
        m_bookmarkAction( 0 ), m_mapInput( 0 ), m_currentLocationAction( 0 ),
        m_centerAction( 0 )
{
    bool const smallScreen = MarbleGlobal::getInstance()->profiles() & MarbleGlobal::SmallScreen;
    int const iconSize = smallScreen ? 32 : 16;

    m_menuButton = new QToolButton( parent );
    m_menuButton->setIcon( QIcon( m_route->pixmap( m_index ) ) );
    m_menuButton->setPopupMode( QToolButton::InstantPopup );
    m_menuButton->setIconSize( QSize( iconSize, iconSize ) );

    m_lineEdit = new RoutingInputLineEdit( parent );

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

void RoutingInputWidgetPrivate::setProgressAnimationEnabled( bool enabled )
{
    if ( enabled ) {
        m_menuButton->setArrowType( Qt::NoArrow );
    } else {
        bool const smallScreen = MarbleGlobal::getInstance()->profiles() & MarbleGlobal::SmallScreen;
        if ( smallScreen ) {
            m_menuButton->setArrowType( Qt::DownArrow );
        } else {
          m_menuButton->setIcon( QIcon( m_route->pixmap( m_index ) ) );
        }
    }
}

RoutingInputWidget::RoutingInputWidget( MarbleWidget* widget, int index, QWidget *parent ) :
        QWidget( parent ), d( new RoutingInputWidgetPrivate( widget, index, this ) )
{
    QHBoxLayout *layout = new QHBoxLayout( this );
    layout->setSpacing( 0 );
    layout->setMargin( 0 );

    bool const smallScreen = MarbleGlobal::getInstance()->profiles() & MarbleGlobal::SmallScreen;
    if ( smallScreen ) {
        layout->addWidget( d->m_lineEdit );
        d->m_menuButton->setArrowType( Qt::DownArrow );
        layout->addWidget( d->m_menuButton );
        connect( d->m_menuButton, SIGNAL( clicked() ), this, SLOT( openTargetSelectionDialog() ) );
    } else {
        d->m_menuButton->setMenu( d->createMenu( this ) );
        layout->addWidget( d->m_menuButton );
        layout->addWidget( d->m_lineEdit );
    }

    connect( d->m_lineEdit, SIGNAL( clearButtonClicked() ), this, SLOT( requestRemoval() ) );
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
        d->setProgressAnimationEnabled( true );
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
    d->setProgressAnimationEnabled( false );
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
    d->setProgressAnimationEnabled( false );
}

void RoutingInputWidget::updatePosition( int index, const GeoDataCoordinates & )
{
    if ( index == d->m_index ) {
        d->setProgressAnimationEnabled( false );
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
    d->setProgressAnimationEnabled( false );
    d->m_route->setPosition( d->m_index, GeoDataCoordinates() );
    d->m_lineEdit->clear();
    emit targetValidityChanged( false );
}

void RoutingInputWidget::retrieveReverseGeocodingResult( const GeoDataCoordinates &coordinates, const GeoDataPlacemark &placemark )
{
    QString description = placemark.address().isEmpty() ? coordinates.toString() : placemark.address();
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
    QPointer<GoToDialog> dialog = new GoToDialog( d->m_marbleWidget, this );
    dialog->setWindowTitle( tr( "Choose Placemark" ) );
    if ( dialog->exec() == QDialog::Accepted ) {
        GeoDataLookAt lookAt = dialog->lookAt();
        setTargetPosition( lookAt.coordinates() );
    }
    delete dialog;
}

} // namespace Marble

#include "RoutingInputWidget.moc"
