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

#include <QtCore/QTimer>
#include <QtCore/QUrl>
#include <QtGui/QHBoxLayout>
#include <QtGui/QIcon>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QtXml/QDomDocument>

namespace Marble
{

class RoutingInputWidgetPrivate
{
public:
    QPushButton *m_stateButton;

    QLineEdit *m_lineEdit;

    QPushButton *m_removeButton;

    QPushButton *m_pickButton;

    MarbleRunnerManager *m_runnerManager;

    MarblePlacemarkModel *m_placemarkModel;

    QTimer m_progressTimer;

    RouteRequest *m_route;

    int m_index;

    QNetworkAccessManager *m_manager;

    QTimer m_nominatimTimer;

    QVector<QIcon> m_progressAnimation;

    int m_currentFrame;

    /** Constructor */
    RoutingInputWidgetPrivate( RouteRequest *request, int index, PluginManager* manager, QWidget *parent );

    /** Initiate reverse geocoding request to download address */
    void adjustText();
};

RoutingInputWidgetPrivate::RoutingInputWidgetPrivate( RouteRequest *request, int index, PluginManager* manager, QWidget *parent ) :
        m_lineEdit( 0 ), m_runnerManager( new MarbleRunnerManager( manager, parent ) ),
        m_placemarkModel( 0 ), m_route( request ), m_index( index ),
        m_manager( new QNetworkAccessManager( parent ) ), m_currentFrame( 0 )
{
    bool smallScreen = MarbleGlobal::getInstance()->profiles() & MarbleGlobal::SmallScreen;
    int const iconSize = smallScreen ? 32 : 16;

    m_stateButton = new QPushButton( parent );
    m_stateButton->setToolTip( QObject::tr( "Center Map here" ) );
    m_stateButton->setVisible( false );
    m_stateButton->setIcon( QIcon( m_route->pixmap( m_index ) ) );
    m_stateButton->setFlat( true );
    m_stateButton->setMaximumWidth( iconSize + 6 );
    m_stateButton->setIconSize( QSize( iconSize, iconSize ) );
    m_stateButton->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed );

    m_lineEdit = new QLineEdit( parent );

    m_removeButton = new QPushButton( parent );
    m_removeButton->setIcon( QIcon( ":/data/bitmaps/routing_remove.png" ) );
    m_removeButton->setToolTip( QObject::tr( "Remove this position" ) );
    m_removeButton->setFlat( true );
    m_removeButton->setMaximumWidth( 22 );

    m_pickButton = new QPushButton( parent );
    m_pickButton->setIcon( QIcon( m_route->pixmap( m_index ) ) );
    m_pickButton->setToolTip( QObject::tr( "Choose position from the map" ) );
    m_pickButton->setCheckable( true );
    m_pickButton->setFlat( true );
    m_pickButton->setMaximumWidth( iconSize + 6 );
    m_pickButton->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed );
    m_pickButton->setIconSize( QSize( iconSize, iconSize ) );

    m_progressTimer.setInterval( 100 );
    m_nominatimTimer.setInterval( 1000 );
    m_nominatimTimer.setSingleShot( true );

    GeoDataCoordinates pos = m_route->at( m_index );
    if ( pos.longitude() != 0.0 && pos.latitude() != 0.0 ) {
        m_pickButton->setVisible( false );
        m_stateButton->setVisible( true );
    }
}

void RoutingInputWidgetPrivate::adjustText()
{
    m_nominatimTimer.start();
}

RoutingInputWidget::RoutingInputWidget( RouteRequest *request, int index, PluginManager* manager, QWidget *parent ) :
        QWidget( parent ), d( new RoutingInputWidgetPrivate( request, index, manager, this ) )
{
    QHBoxLayout *layout = new QHBoxLayout( this );
    layout->setSpacing( 0 );
    layout->setMargin( 0 );

    layout->addWidget( d->m_pickButton );
    layout->addWidget( d->m_stateButton );
    layout->addWidget( d->m_lineEdit );
    layout->addWidget( d->m_removeButton );

    connect( d->m_stateButton, SIGNAL( clicked() ),
             this, SLOT( requestActivity() ) );
    connect( d->m_pickButton, SIGNAL( clicked( bool ) ),
             this, SLOT( setMapInputModeEnabled( bool ) ) );
    connect( d->m_removeButton, SIGNAL( clicked() ),
             this, SLOT( requestRemoval() ) );

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
    connect( request, SIGNAL( positionChanged( int, GeoDataCoordinates ) ),
             this, SLOT( updatePosition( int, GeoDataCoordinates ) ) );
    connect( &d->m_nominatimTimer, SIGNAL( timeout() ),
             this, SLOT( reverseGeocoding() ) );

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
    d->m_pickButton->setVisible( false );
    d->m_route->setPosition( d->m_index, position );
    d->m_progressTimer.stop();
    d->m_stateButton->setVisible( true );
    d->m_stateButton->setIcon( QIcon( d->m_route->pixmap( d->m_index ) ) );
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
        d->m_pickButton->setVisible( false );
        d->m_stateButton->setVisible( true );
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
        d->m_stateButton->setIcon( frame );
    }
}

void RoutingInputWidget::finishSearch()
{
    d->m_progressTimer.stop();
    d->m_stateButton->setIcon( QIcon( d->m_route->pixmap( d->m_index ) ) );
    d->m_pickButton->setIcon( QIcon( d->m_route->pixmap( d->m_index ) ) );
    d->m_pickButton->setVisible( false );
    d->m_stateButton->setVisible( true );
    emit searchFinished( this );
}

void RoutingInputWidget::setInvalid()
{
    d->m_route->setPosition( d->m_index, GeoDataCoordinates() );
    emit targetValidityChanged( false );
}

void RoutingInputWidget::abortMapInputRequest()
{
    d->m_pickButton->setChecked( false );
}

void RoutingInputWidget::setIndex( int index )
{
    d->m_index = index;
    d->m_stateButton->setVisible( hasTargetPosition() );
    d->m_stateButton->setIcon( QIcon( d->m_route->pixmap( d->m_index ) ) );
    d->m_pickButton->setIcon( QIcon( d->m_route->pixmap( d->m_index ) ) );
}

void RoutingInputWidget::updatePosition( int index, const GeoDataCoordinates & )
{
    if ( index == d->m_index ) {
        d->m_stateButton->setVisible( hasTargetPosition() );
        d->m_stateButton->setIcon( d->m_route->pixmap( d->m_index ) );
        d->m_pickButton->setVisible( !hasTargetPosition() );
        d->m_pickButton->setIcon( d->m_route->pixmap( d->m_index ) );
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
    d->m_pickButton->setChecked( false );
    d->m_pickButton->setVisible( true );
    d->m_stateButton->setVisible( false );
    d->m_stateButton->setIcon( d->m_route->pixmap( d->m_index ) );
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

} // namespace Marble

#include "RoutingInputWidget.moc"
