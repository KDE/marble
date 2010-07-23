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

#include "MarbleDebug.h"
#include "MarbleLocale.h"
#include "MarblePlacemarkModel.h"
#include "MarbleRunnerManager.h"
#include "RouteSkeleton.h"
#include "TinyWebBrowser.h"

#include <QtCore/QTimer>
#include <QtCore/QUrl>
#include <QtGui/QHBoxLayout>
#include <QtGui/QIcon>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>
#include <QtGui/QMovie>
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

    QMovie m_progress;

    QTimer m_progressTimer;

    RouteSkeleton *m_route;

    int m_index;

    QNetworkAccessManager *m_manager;

    QTimer m_nominatimTimer;

    bool m_workOffline;

    /** Constructor */
    RoutingInputWidgetPrivate( RouteSkeleton *skeleton, int index, PluginManager* manager, QWidget *parent );

    /** Initiate reverse geocoding request to download address */
    void adjustText();
};

RoutingInputWidgetPrivate::RoutingInputWidgetPrivate( RouteSkeleton *skeleton, int index, PluginManager* manager, QWidget *parent ) :
        m_lineEdit( 0 ), m_runnerManager( new MarbleRunnerManager( manager, parent ) ),
        m_placemarkModel( 0 ), m_progress( ":/data/bitmaps/progress.mng" ),
        m_route( skeleton ), m_index( index ), m_manager( new QNetworkAccessManager( parent ) ),
        m_workOffline( false )
{
    m_stateButton = new QPushButton( parent );
    m_stateButton->setToolTip( "Center Map here" );
    m_stateButton->setVisible( false );
    m_stateButton->setIcon( QIcon( m_route->pixmap( m_index ) ) );
    m_stateButton->setFlat( true );
    m_stateButton->setMaximumWidth( 22 );
    m_stateButton->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed );

    m_lineEdit = new QLineEdit( parent );

    m_removeButton = new QPushButton( parent );
    m_removeButton->setIcon( QIcon( ":/data/bitmaps/routing_remove.png" ) );
    m_removeButton->setToolTip( "Remove this position" );
    m_removeButton->setFlat( true );
    m_removeButton->setMaximumWidth( 22 );

    m_pickButton = new QPushButton( parent );
    m_pickButton->setIcon( QIcon( m_route->pixmap( m_index ) ) );
    m_pickButton->setToolTip( "Choose position from the map" );
    m_pickButton->setCheckable( true );
    m_pickButton->setFlat( true );
    m_pickButton->setMaximumWidth( 22 );
    m_pickButton->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed );

    m_progressTimer.setInterval( 100 );
    m_nominatimTimer.setInterval( 1000 );
    m_nominatimTimer.setSingleShot( true );

    GeoDataCoordinates pos = m_route->at( m_index );
    if ( pos.longitude() != 0.0 && pos.latitude() != 0.0 ) {
        m_lineEdit->setText( pos.toString() );
        m_pickButton->setVisible( false );
        m_stateButton->setVisible( true );
    }
}

void RoutingInputWidgetPrivate::adjustText()
{
    m_nominatimTimer.start();
}

RoutingInputWidget::RoutingInputWidget( RouteSkeleton *skeleton, int index, PluginManager* manager, QWidget *parent ) :
        QWidget( parent ), d( new RoutingInputWidgetPrivate( skeleton, index, manager, this ) )
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
    connect( d->m_runnerManager, SIGNAL( reverseGeocodingFinished( GeoDataPlacemark )),
             this, SLOT(retrieveReverseGeocodingResult( GeoDataPlacemark ) ) );
    connect( d->m_lineEdit, SIGNAL( returnPressed() ),
             this, SLOT( findPlacemarks() ) );
    connect( d->m_lineEdit, SIGNAL( textChanged( QString ) ),
             this, SLOT( setInvalid() ) );
    connect( &d->m_progressTimer, SIGNAL( timeout() ),
             this, SLOT( updateProgress() ) );
    connect( d->m_runnerManager, SIGNAL( searchFinished( QString ) ),
             this, SLOT( finishSearch() ) );
    connect( skeleton, SIGNAL( positionChanged( int, GeoDataCoordinates ) ),
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
    if ( d->m_workOffline || !hasTargetPosition() ) {
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
    if ( !hasInput() || d->m_pickButton->isChecked() ) {
        d->m_lineEdit->setText( position.toString() );
    }

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
    d->m_progress.jumpToNextFrame();
    QPixmap frame = d->m_progress.currentPixmap();
    d->m_stateButton->setIcon( frame );
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
    if ( !hasTargetPosition() ) {
        emit targetValidityChanged( false );
    }
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

void RoutingInputWidget::updatePosition( int index, const GeoDataCoordinates &position )
{
    if ( index == d->m_index ) {
        d->m_lineEdit->setText( position.toString() );
        d->m_stateButton->setVisible( hasTargetPosition() );
        d->m_stateButton->setIcon( d->m_route->pixmap( d->m_index ) );
        d->m_pickButton->setIcon( d->m_route->pixmap( d->m_index ) );
        emit targetValidityChanged( hasTargetPosition() );
        d->adjustText();
    }
}

void RoutingInputWidget::setWorkOffline( bool offline )
{
    d->m_workOffline = offline;
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

void RoutingInputWidget::retrieveReverseGeocodingResult( const GeoDataPlacemark &placemark )
{
    d->m_lineEdit->setText( placemark.address() );
    d->m_lineEdit->setCursorPosition( 0 );
}

} // namespace Marble

#include "RoutingInputWidget.moc"
