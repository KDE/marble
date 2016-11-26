//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010      Dennis Nienhüser  <nienhueser@kde.org>
// Copyright 2012      Illya Kovalevskyy <illya.kovalevskyy@gmail.com>
//

#include "RoutingInputWidget.h"

#include "MarblePlacemarkModel.h"
#include "RouteRequest.h"

#ifdef MARBLE_NO_WEBKITWIDGETS
#include "NullTinyWebBrowser.h"
#else
#include "TinyWebBrowser.h"
#endif

#include "BookmarkManager.h"
#include "MarbleModel.h"
#include "MarbleWidget.h"
#include "routing/RoutingManager.h"
#include "GeoDataPlacemark.h"
#include "GeoDataFolder.h"
#include "GeoDataExtendedData.h"
#include "GeoDataData.h"
#include "PositionTracking.h"
#include "ReverseGeocodingRunnerManager.h"
#include "SearchRunnerManager.h"
#include "MarbleLineEdit.h"
#include "GoToDialog.h"

#include <QTimer>
#include <QHBoxLayout>
#include <QIcon>
#include <QPushButton>
#include <QMenu>
#include <QKeyEvent>
#include <QPainter>

namespace Marble
{

/**
  * A MarbleLineEdit that swallows enter/return pressed
  * key events
  */
class RoutingInputLineEdit : public MarbleLineEdit
{
public:
    explicit RoutingInputLineEdit( QWidget *parent = 0 );

protected:
    virtual void keyPressEvent(QKeyEvent *);
};

class RoutingInputWidgetPrivate
{
public:
    MarbleModel* m_marbleModel;

    RoutingInputLineEdit *m_lineEdit;

    QPushButton* m_removeButton;

    SearchRunnerManager m_placemarkRunnerManager;
    ReverseGeocodingRunnerManager m_reverseGeocodingRunnerManager;

    MarblePlacemarkModel *m_placemarkModel;

    RouteRequest *m_route;

    int m_index;

    QTimer m_nominatimTimer;

    QAction* m_bookmarkAction;

    QAction* m_mapInput;

    QAction* m_currentLocationAction;

    QAction* m_centerAction;

    QMenu *m_menu;

    /** Constructor */
    RoutingInputWidgetPrivate( MarbleModel* model, int index, QWidget *parent );

    /** Initiate reverse geocoding request to download address */
    void adjustText();

    void createMenu( RoutingInputWidget *parent );

    QMenu* createBookmarkMenu( RoutingInputWidget *parent );

    static void createBookmarkActions( QMenu* menu, GeoDataFolder* bookmarksFolder, QObject *parent );

    static QPixmap addDropDownIndicator( const QPixmap &pixmap );

    void updateDescription();
};

void RoutingInputWidgetPrivate::updateDescription()
{
    GeoDataPlacemark const placemark = (*m_route)[m_index];
    GeoDataExtendedData const address = placemark.extendedData();
    if (address.contains(QStringLiteral("road")) && address.contains(QStringLiteral("city"))) {
        QString const road = address.value(QStringLiteral("road")).value().toString();
        QString const city = address.value(QStringLiteral("city")).value().toString();

        if (address.contains(QStringLiteral("house_number"))) {
            QString const houseNumber = address.value(QStringLiteral("house_number")).value().toString();
            QString const name = QObject::tr("%1 %2, %3", "An address with parameters %1=house number, %2=road, %3=city");
            m_lineEdit->setText( name.arg( houseNumber ).arg( road ).arg( city ) );
        } else {
            QString const name = QObject::tr("%2, %3", "An address with parameters %1=road, %2=city");
            m_lineEdit->setText( name.arg( road ).arg( city ) );
        }
    }
    else if ( m_route->name( m_index ).isEmpty() )
    {
        if ( !placemark.address().isEmpty() ) {
            m_lineEdit->setText( placemark.address() );
        }
        else {
            m_lineEdit->setText( placemark.coordinate().toString().trimmed() );
        }
    }
    else
    {
        m_lineEdit->setText( placemark.name() );
    }
    m_lineEdit->setCursorPosition( 0 );
}

RoutingInputLineEdit::RoutingInputLineEdit( QWidget *parent ) :
    MarbleLineEdit( parent )
{
    setPlaceholderText( QObject::tr( "Address or search term..." ) );
}

void RoutingInputLineEdit::keyPressEvent(QKeyEvent *event)
{
    MarbleLineEdit::keyPressEvent( event );
    bool const returnPressed = event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter;
    if ( returnPressed ) {
        event->accept();
    }
}

RoutingInputWidgetPrivate::RoutingInputWidgetPrivate( MarbleModel* model, int index, QWidget *parent ) :
        m_marbleModel( model ),
        m_lineEdit( 0 ),
        m_placemarkRunnerManager( m_marbleModel ),
        m_reverseGeocodingRunnerManager( m_marbleModel ),
        m_placemarkModel( 0 ), m_route( m_marbleModel->routingManager()->routeRequest() ), m_index( index ),
        m_bookmarkAction( 0 ), m_mapInput( 0 ), m_currentLocationAction( 0 ),
        m_centerAction( 0 ),
        m_menu( 0 )
{
    m_lineEdit = new RoutingInputLineEdit( parent );
    m_lineEdit->setDecorator( addDropDownIndicator( m_route->pixmap( m_index ) ) );

    m_removeButton = new QPushButton( parent );
    m_removeButton->setIcon(QIcon(QStringLiteral(":/marble/routing/icon-remove.png")));
    m_removeButton->setToolTip( QObject::tr( "Remove via point" ) );
    m_removeButton->setFlat( true );
    m_removeButton->setMaximumWidth( 18 );

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
                       parent, SLOT(requestActivity()) );
    result->addSeparator();

    m_currentLocationAction = result->addAction( QIcon(QStringLiteral(":/icons/gps.png")), QObject::tr("Current &Location"),
                                                 parent, SLOT(setCurrentLocation()) );
    m_currentLocationAction->setEnabled( false );

    m_mapInput = result->addAction(QIcon(QStringLiteral(":/icons/crosshairs.png")), QObject::tr("From &Map..."));
    m_mapInput->setCheckable( true );
    QObject::connect( m_mapInput, SIGNAL(triggered(bool)), parent, SLOT(setMapInputModeEnabled(bool)) );

    m_bookmarkAction = result->addAction(QIcon(QStringLiteral(":/icons/bookmarks.png")), QObject::tr("From &Bookmark"));
    m_bookmarkAction->setMenu( createBookmarkMenu( parent ) );

    m_menu = result;
}

QMenu* RoutingInputWidgetPrivate::createBookmarkMenu( RoutingInputWidget *parent )
{
    QMenu* result = new QMenu( parent );
    result->addAction(QIcon(QStringLiteral(":/icons/go-home.png")), QObject::tr("&Home"), parent, SLOT(setHomePosition()));

    QVector<GeoDataFolder*> folders = m_marbleModel->bookmarkManager()->folders();

    if ( folders.size() == 1 ) {
        createBookmarkActions( result, folders.first(), parent );
    } else {
        QVector<GeoDataFolder*>::const_iterator i = folders.constBegin();
        QVector<GeoDataFolder*>::const_iterator end = folders.constEnd();

        for (; i != end; ++i ) {
            QMenu* menu = result->addMenu(QIcon(QStringLiteral(":/icons/folder-bookmark.png")), (*i)->name());
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
        QObject::connect( menu, SIGNAL(triggered(QAction*)), parent, SLOT(setBookmarkPosition(QAction*)) );
    }
}

QPixmap RoutingInputWidgetPrivate::addDropDownIndicator(const QPixmap &pixmap)
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

RoutingInputWidget::RoutingInputWidget( MarbleModel* model, int index, QWidget *parent ) :
        QWidget( parent ), d( new RoutingInputWidgetPrivate( model, index, this ) )
{
    QHBoxLayout *layout = new QHBoxLayout( this );
    layout->setSizeConstraint( QLayout::SetMinimumSize );
    layout->setSpacing( 0 );
    layout->setMargin( 0 );
    layout->addWidget( d->m_lineEdit );
    layout->addWidget( d->m_removeButton );

    bool const smallScreen = MarbleGlobal::getInstance()->profiles() & MarbleGlobal::SmallScreen;
    if ( smallScreen ) {
        connect( d->m_lineEdit, SIGNAL(decoratorButtonClicked()), this, SLOT(openTargetSelectionDialog()) );
    } else {
        d->createMenu( this );
        connect(d->m_lineEdit, SIGNAL(decoratorButtonClicked()), this, SLOT(showMenu()));
    }

    connect( d->m_removeButton, SIGNAL(clicked()), this, SLOT(requestRemoval()) );
    connect( d->m_marbleModel->bookmarkManager(), SIGNAL(bookmarksChanged()),
             this, SLOT(reloadBookmarks()) );
    connect( d->m_marbleModel->positionTracking(), SIGNAL(statusChanged(PositionProviderStatus)),
             this, SLOT(updateCurrentLocationButton(PositionProviderStatus)) );
    connect( &d->m_placemarkRunnerManager, SIGNAL(searchResultChanged(QAbstractItemModel*)),
             this, SLOT(setPlacemarkModel(QAbstractItemModel*)) );
    connect( &d->m_reverseGeocodingRunnerManager, SIGNAL(reverseGeocodingFinished(GeoDataCoordinates,GeoDataPlacemark)),
             this, SLOT(retrieveReverseGeocodingResult(GeoDataCoordinates,GeoDataPlacemark)) );
    connect( d->m_lineEdit, SIGNAL(returnPressed()),
             this, SLOT(findPlacemarks()) );
    connect( d->m_lineEdit, SIGNAL(textEdited(QString)),
             this, SLOT(setInvalid()) );
    connect( &d->m_placemarkRunnerManager, SIGNAL(searchFinished(QString)),
             this, SLOT(finishSearch()) );
    connect( d->m_marbleModel->routingManager()->routeRequest(), SIGNAL(positionChanged(int,GeoDataCoordinates)),
             this, SLOT(updatePosition(int,GeoDataCoordinates)) );
    connect( &d->m_nominatimTimer, SIGNAL(timeout()),
             this, SLOT(reverseGeocoding()) );
    connect( this, SIGNAL(targetValidityChanged(bool)), this, SLOT(updateCenterButton(bool)) );
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
        d->m_reverseGeocodingRunnerManager.reverseGeocoding( targetPosition() );
    } else {
        d->updateDescription();
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
        d->updateDescription();
    }
    emit targetValidityChanged( true );
}

bool RoutingInputWidget::hasTargetPosition() const
{
    return targetPosition().isValid();
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
        d->m_placemarkRunnerManager.findPlacemarks( text );
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

void RoutingInputWidget::retrieveReverseGeocodingResult( const GeoDataCoordinates &, const GeoDataPlacemark &placemark )
{
    (*d->m_route)[d->m_index] = placemark;
    d->updateDescription();
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
        setTargetPosition( bookmark->data().value<GeoDataCoordinates>() );
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

#include "moc_RoutingInputWidget.cpp"
