//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010      Dennis Nienhüser <earthwings@gentoo.org>
//

#include "RoutingWidget.h"

#include "GeoDataLineString.h"
#include "MarbleModel.h"
#include "MarblePlacemarkModel.h"
#include "MarbleWidget.h"
#include "MarbleWidgetInputHandler.h"
#include "RouteRequest.h"
#include "RoutingInputWidget.h"
#include "RoutingLayer.h"
#include "RoutingManager.h"
#include "RoutingModel.h"
#include "RoutingProfilesModel.h"
#include "RoutingProfileSettingsDialog.h"
#include "GeoDataDocument.h"
#include "AlternativeRoutesModel.h"

#include <QtCore/QTime>
#include <QtCore/QTimer>
#include <QtGui/QSortFilterProxyModel>
#include <QtGui/QComboBox>
#include <QtGui/QPainter>
#include <QtGui/QFileDialog>

#include "ui_RoutingWidget.h"

namespace Marble
{

class RoutingWidgetPrivate
{
public:
    Ui::RoutingWidget m_ui;

    MarbleWidget *const m_widget;

    RoutingManager *const m_routingManager;

    RoutingLayer *const m_routingLayer;

    RoutingInputWidget *m_activeInput;

    QVector<RoutingInputWidget*> m_inputWidgets;

    RoutingInputWidget *m_inputRequest;

    QAbstractItemModel *const m_routingModel;

    RouteRequest *const m_routeRequest;

    bool m_zoomRouteAfterDownload;

    QTimer m_progressTimer;

    QVector<QIcon> m_progressAnimation;

    int m_currentFrame;

    int m_iconSize;

    int m_collapse_width;

    /** Constructor */
    RoutingWidgetPrivate( MarbleWidget *marbleWidget );

    /**
      * @brief Toggle between simple search view and route view
      * If only one input field exists, hide all buttons
      */
    void adjustInputWidgets();

    void adjustSearchButton();

    /**
      * @brief Change the active input widget
      * The active input widget influences what is shown in the paint layer
      * and in the list view: Either a set of placemarks that correspond to
      * a runner search result or the current route
      */
    void setActiveInput( RoutingInputWidget* widget );

private:
    void createProgressAnimation();
};

RoutingWidgetPrivate::RoutingWidgetPrivate( MarbleWidget *marbleWidget ) :
        m_widget( marbleWidget ),
        m_routingManager( marbleWidget->model()->routingManager() ),
        m_routingLayer( marbleWidget->routingLayer() ),
        m_activeInput( 0 ),
        m_inputRequest( 0 ),
        m_routingModel( m_routingManager->routingModel() ),
        m_routeRequest( marbleWidget->model()->routingManager()->routeRequest() ),
        m_zoomRouteAfterDownload( false ),
        m_currentFrame( 0 ),
        m_iconSize( 16 ),
        m_collapse_width( 0 )
{
    createProgressAnimation();
    m_progressTimer.setInterval( 100 );
    if ( MarbleGlobal::getInstance()->profiles() & MarbleGlobal::SmallScreen ) {
        m_iconSize = 32;
    }
}

void RoutingWidgetPrivate::adjustInputWidgets()
{
    for ( int i = 0; i < m_inputWidgets.size(); ++i ) {
        m_inputWidgets[i]->setIndex( i );
    }

    adjustSearchButton();
}

void RoutingWidgetPrivate::adjustSearchButton()
{
    QString text = QObject::tr( "Get Directions" );
    QString tooltip = QObject::tr( "Retrieve routing instructions for the selected destinations." );

    int validInputs = 0;
    for ( int i = 0; i < m_inputWidgets.size(); ++i ) {
        if ( m_inputWidgets[i]->hasTargetPosition() ) {
            ++validInputs;
        }
    }

    if ( validInputs < 2 ) {
        text = QObject::tr( "Search" );
        tooltip = QObject::tr( "Find places matching the search term" );
    }

    m_ui.searchButton->setText( text );
    m_ui.searchButton->setToolTip( tooltip );
}

void RoutingWidgetPrivate::setActiveInput( RoutingInputWidget *widget )
{
    Q_ASSERT( widget && "Must not pass null" );
    MarblePlacemarkModel *model = widget->searchResultModel();

    m_activeInput = widget;
    m_ui.directionsListView->setModel( model );
    m_routingLayer->setPlacemarkModel( model );
    m_routingLayer->synchronizeWith( m_ui.directionsListView->selectionModel() );
}

void RoutingWidgetPrivate::createProgressAnimation()
{
    // Size parameters
    qreal const h = m_iconSize / 2.0; // Half of the icon size
    qreal const q = h / 2.0; // Quarter of the icon size
    qreal const d = 7.5; // Circle diameter
    qreal const r = d / 2.0; // Circle radius

    // Canvas parameters
    QImage canvas( m_iconSize, m_iconSize, QImage::Format_ARGB32 );
    QPainter painter( &canvas );
    painter.setRenderHint( QPainter::Antialiasing, true );
    painter.setPen( QColor ( Qt::gray ) );
    painter.setBrush( QColor( Qt::white ) );

    // Create all frames
    for( double t = 0.0; t < 2 * M_PI; t += M_PI / 8.0 ) {
        canvas.fill( Qt::transparent );
        QRectF firstCircle( h - r + q * cos( t ), h - r + q * sin( t ), d, d );
        QRectF secondCircle( h - r + q * cos( t + M_PI ), h - r + q * sin( t + M_PI ), d, d );
        painter.drawEllipse( firstCircle );
        painter.drawEllipse( secondCircle );
        m_progressAnimation.push_back( QIcon( QPixmap::fromImage( canvas ) ) );
    }
}

RoutingWidget::RoutingWidget( MarbleWidget *marbleWidget, QWidget *parent ) :
    QWidget( parent ), d( new RoutingWidgetPrivate( marbleWidget ) )
{
    d->m_ui.setupUi( this );
    d->m_ui.routeComboBox->setVisible( false );
    d->m_ui.routeComboBox->setModel( d->m_routingManager->alternativeRoutesModel() );

    d->m_routingLayer->synchronizeAlternativeRoutesWith( d->m_ui.routeComboBox );

    d->m_ui.routingProfileComboBox->setModel( d->m_routingManager->profilesModel() );

    connect( d->m_routingManager->profilesModel(), SIGNAL( rowsInserted( QModelIndex, int, int ) ),
             this, SLOT( selectFirstProfile() ) );
    connect( d->m_routingManager->profilesModel(), SIGNAL( modelReset() ),
             this, SLOT( selectFirstProfile() ) );
    connect( d->m_routingLayer, SIGNAL( placemarkSelected( QModelIndex ) ),
             this, SLOT( activatePlacemark( QModelIndex ) ) );
    connect( d->m_routingLayer, SIGNAL( pointSelected( GeoDataCoordinates ) ),
             this, SLOT( retrieveSelectedPoint( GeoDataCoordinates ) ) );
    connect( d->m_routingLayer, SIGNAL( pointSelectionAborted() ),
             this, SLOT( pointSelectionCanceled() ) );
    connect( d->m_routingManager, SIGNAL( stateChanged( RoutingManager::State ) ),
             this, SLOT( updateRouteState( RoutingManager::State ) ) );
    connect( d->m_routingManager, SIGNAL( routeRetrieved( GeoDataDocument* ) ),
             this, SLOT( indicateRoutingFailure( GeoDataDocument* ) ) );
    connect( d->m_routeRequest, SIGNAL( positionAdded( int ) ),
             this, SLOT( insertInputWidget( int ) ) );
    connect( d->m_routeRequest, SIGNAL( positionRemoved( int ) ),
             this, SLOT( removeInputWidget( int ) ) );
    connect( d->m_routeRequest, SIGNAL( routingProfileChanged() ),
             this, SLOT( updateActiveRoutingProfile() ) );
    connect( &d->m_progressTimer, SIGNAL( timeout() ),
             this, SLOT( updateProgress() ) );
    connect( d->m_ui.routeComboBox, SIGNAL( currentIndexChanged( int ) ),
             this, SLOT( switchRoute( int ) ) );
    connect( d->m_ui.routingProfileComboBox, SIGNAL( currentIndexChanged( int ) ),
             this, SLOT( setRoutingProfile( int ) ) );
    connect( d->m_ui.routingProfileComboBox, SIGNAL( activated( int ) ),
             this, SLOT( retrieveRoute() ) );
    connect( d->m_routingManager->alternativeRoutesModel(), SIGNAL( rowsInserted( QModelIndex, int, int ) ),
             this, SLOT( updateAlternativeRoutes() ) );

    d->m_ui.directionsListView->setModel( d->m_routingModel );

    QItemSelectionModel *selectionModel = d->m_ui.directionsListView->selectionModel();
    d->m_routingLayer->synchronizeWith( selectionModel );
    connect( d->m_ui.directionsListView, SIGNAL( activated ( QModelIndex ) ),
             this, SLOT( activateItem ( QModelIndex ) ) );

    connect( d->m_ui.openRouteButton, SIGNAL( clicked() ),
             this, SLOT( openRoute () ) );
    connect( d->m_ui.saveRouteButton, SIGNAL( clicked() ),
             this, SLOT( saveRoute () ) );
    connect( d->m_ui.addViaButton, SIGNAL( clicked() ),
             this, SLOT( addInputWidget() ) );
    connect( d->m_ui.reverseRouteButton, SIGNAL( clicked() ),
             d->m_routingManager, SLOT( reverseRoute () ) );
    connect( d->m_ui.clearRouteButton, SIGNAL( clicked() ),
             d->m_routingManager, SLOT( clearRoute () ) );
    connect( d->m_ui.searchButton, SIGNAL( clicked() ),
             this, SLOT( retrieveRoute () ) );
    connect( d->m_ui.showInstructionsButton, SIGNAL( clicked( bool ) ),
             this, SLOT( showDirections() ) );
    connect( d->m_ui.configureButton, SIGNAL( clicked() ),
             this, SLOT( configureProfile() ) );

    for( int i=0; i<d->m_routeRequest->size(); ++i ) {
        insertInputWidget( i );
    }

    for ( int i=0; i<2 && d->m_inputWidgets.size()<2; ++i ) {
        // Start with source and destination if the route is empty yet
        addInputWidget();
    }
    //d->m_ui.descriptionLabel->setVisible( false );
    d->m_ui.resultLabel->setVisible( false );
    setShowDirectionsButtonVisible( false );
    updateActiveRoutingProfile();

    if ( MarbleGlobal::getInstance()->profiles() & MarbleGlobal::SmallScreen ) {
        d->m_ui.directionsListView->setVisible( false );
        d->m_ui.openRouteButton->setVisible( false );
        d->m_ui.saveRouteButton->setVisible( false );
#ifdef Q_WS_MAEMO_5
        d->m_ui.directionsListView->setAttribute( Qt::WA_Maemo5StackedWindow );
        d->m_ui.directionsListView->setWindowFlags( Qt::Window );
        d->m_ui.directionsListView->setWindowTitle( tr( "Directions - Marble" ) );
#endif // Q_WS_MAEMO_5
    }
}

RoutingWidget::~RoutingWidget()
{
    delete d;
}

void RoutingWidget::retrieveRoute()
{
    if ( d->m_inputWidgets.size() == 1 ) {
        // Search mode
        d->m_inputWidgets.first()->findPlacemarks();
        return;
    }

    int index = d->m_ui.routingProfileComboBox->currentIndex();
    if ( index == -1 ) {
        return;
    }
    d->m_routeRequest->setRoutingProfile( d->m_routingManager->profilesModel()->profiles().at( index ) );

    Q_ASSERT( d->m_routeRequest->size() == d->m_inputWidgets.size() );
    for ( int i = 0; i < d->m_inputWidgets.size(); ++i ) {
        RoutingInputWidget *widget = d->m_inputWidgets.at( i );
        if ( !widget->hasTargetPosition() && widget->hasInput() ) {
            widget->findPlacemarks();
            return;
        }
    }

    d->m_activeInput = 0;
    if ( d->m_routeRequest->size() > 1 ) {
        d->m_zoomRouteAfterDownload = true;
        d->m_routingLayer->setPlacemarkModel( 0 );
        d->m_routingManager->retrieveRoute();
        d->m_ui.directionsListView->setModel( d->m_routingModel );
        d->m_routingLayer->synchronizeWith( d->m_ui.directionsListView->selectionModel() );
    }
}

void RoutingWidget::activateItem ( const QModelIndex &index )
{
    QVariant data = index.data( MarblePlacemarkModel::CoordinateRole );

    if ( !data.isNull() ) {
        GeoDataCoordinates position = qvariant_cast<GeoDataCoordinates>( data );
        d->m_widget->centerOn( position, true );
    }

    if ( d->m_activeInput && index.isValid() ) {
        QVariant data = index.data( MarblePlacemarkModel::CoordinateRole );
        if ( !data.isNull() ) {
            d->m_activeInput->setTargetPosition( qVariantValue<GeoDataCoordinates>( data), index.data().toString() );
        }
    }
}

void RoutingWidget::handleSearchResult( RoutingInputWidget *widget )
{
    d->setActiveInput( widget );
    MarblePlacemarkModel *model = widget->searchResultModel();

    if ( model->rowCount() ) {
        QString const results = tr( "%n placemarks found", "", model->rowCount() );
        d->m_ui.resultLabel->setText( results );
        d->m_ui.resultLabel->setVisible( true );
        // Make sure we have a selection
        activatePlacemark( model->index( 0, 0 ) );
    } else {
        QString const results = tr( "No placemark found" );
        d->m_ui.resultLabel->setText( "<font color=\"red\">" + results + "</font>" );
        d->m_ui.resultLabel->setVisible( true );
    }

    GeoDataLineString placemarks;
    for ( int i = 0; i < model->rowCount(); ++i ) {
        QVariant data = model->index( i, 0 ).data( MarblePlacemarkModel::CoordinateRole );
        if ( !data.isNull() ) {
            placemarks << qVariantValue<GeoDataCoordinates>( data );
        }
    }

    if ( placemarks.size() > 1 ) {
        d->m_widget->centerOn( GeoDataLatLonBox::fromLineString( placemarks ) );
        //d->m_ui.descriptionLabel->setVisible( false );

        if ( MarbleGlobal::getInstance()->profiles() & MarbleGlobal::SmallScreen ) {
            d->m_ui.directionsListView->setVisible( true );
        }
    }
}

void RoutingWidget::centerOnInputWidget( RoutingInputWidget *widget )
{
    if ( widget->hasTargetPosition() ) {
        d->m_widget->centerOn( widget->targetPosition() );
    }
}

void RoutingWidget::activatePlacemark( const QModelIndex &index )
{
    if ( d->m_activeInput && index.isValid() ) {
        QVariant data = index.data( MarblePlacemarkModel::CoordinateRole );
        if ( !data.isNull() ) {
            d->m_activeInput->setTargetPosition( qVariantValue<GeoDataCoordinates>( data) );
        }
    }

    d->m_ui.directionsListView->setCurrentIndex( index );
}

void RoutingWidget::addInputWidget()
{
    d->m_routeRequest->append( GeoDataCoordinates() );
}

void RoutingWidget::insertInputWidget( int index )
{
    if ( index >= 0 && index <= d->m_inputWidgets.size() ) {
        RoutingInputWidget *input = new RoutingInputWidget( d->m_widget, index, this );
        input->setProgressAnimation( d->m_progressAnimation );
        d->m_inputWidgets.insert( index, input );
        connect( input, SIGNAL( searchFinished( RoutingInputWidget* ) ),
                 this, SLOT( handleSearchResult( RoutingInputWidget* ) ) );
        connect( input, SIGNAL( removalRequest( RoutingInputWidget* ) ),
                 this, SLOT( removeInputWidget( RoutingInputWidget* ) ) );
        connect( input, SIGNAL( activityRequest( RoutingInputWidget* ) ),
                 this, SLOT( centerOnInputWidget( RoutingInputWidget* ) ) );
        connect( input, SIGNAL( mapInputModeEnabled( RoutingInputWidget*, bool ) ),
                 this, SLOT( requestMapPosition( RoutingInputWidget*, bool ) ) );
        connect( input, SIGNAL( targetValidityChanged( bool ) ),
                 this, SLOT( adjustSearchButton() ) );

        d->m_ui.routingLayout->insertWidget( index, input );
        d->adjustInputWidgets();
    }
}

void RoutingWidget::removeInputWidget( RoutingInputWidget *widget )
{
    int index = d->m_inputWidgets.indexOf( widget );
    if ( index >= 0 ) {
        if ( d->m_inputWidgets.size() < 3 ) {
            widget->clear();
        } else {
            d->m_routeRequest->remove( index );
        }
        d->m_routingManager->retrieveRoute();
    }
}

void RoutingWidget::removeInputWidget( int index )
{
    if ( index >= 0 && index < d->m_inputWidgets.size() ) {
        RoutingInputWidget *widget = d->m_inputWidgets.at( index );
        d->m_inputWidgets.remove( index );
        d->m_ui.routingLayout->removeWidget( widget );
        widget->deleteLater();
        if ( widget == d->m_activeInput ) {
            d->m_activeInput = 0;
            d->m_routingLayer->setPlacemarkModel( 0 );
            d->m_ui.directionsListView->setModel( d->m_routingModel );
            d->m_routingLayer->synchronizeWith( d->m_ui.directionsListView->selectionModel() );
        }
        d->adjustInputWidgets();
    }

    if ( d->m_inputWidgets.size() < 2 ) {
        addInputWidget();
    }
}

void RoutingWidget::updateRouteState( RoutingManager::State state )
{
    if ( state != RoutingManager::Retrieved ) {
        d->m_ui.routeComboBox->setVisible( false );
        d->m_ui.routeComboBox->clear();
    }

    if ( state == RoutingManager::Downloading ) {
        d->m_progressTimer.start();
    }

    d->m_ui.saveRouteButton->setEnabled( d->m_routingManager->routingModel()->rowCount() > 0 );
}

void RoutingWidget::requestMapPosition( RoutingInputWidget *widget, bool enabled )
{
    pointSelectionCanceled();

    if ( enabled ) {
        d->m_inputRequest = widget;
        d->m_routingLayer->setPointSelectionEnabled( true );
        d->m_widget->setFocus( Qt::OtherFocusReason );
    } else {
        d->m_routingLayer->setPointSelectionEnabled( false );
    }
}

void RoutingWidget::retrieveSelectedPoint( const GeoDataCoordinates &coordinates )
{
    if ( d->m_inputRequest && d->m_inputWidgets.contains( d->m_inputRequest ) ) {
        d->m_inputRequest->setTargetPosition( coordinates );
        d->m_inputRequest = 0;
        d->m_widget->update();
    }

    d->m_routingLayer->setPointSelectionEnabled( false );
}

void RoutingWidget::adjustSearchButton()
{
    d->adjustSearchButton();
}

void RoutingWidget::pointSelectionCanceled()
{
    if ( d->m_inputRequest ) {
        d->m_inputRequest->abortMapInputRequest();
    }
}

void RoutingWidget::configureProfile()
{
    int index = d->m_ui.routingProfileComboBox->currentIndex();
    if ( index != -1 ) {
        RoutingProfileSettingsDialog dialog( d->m_widget->model()->pluginManager(), d->m_routingManager->profilesModel(), this );
        dialog.editProfile( d->m_ui.routingProfileComboBox->currentIndex() );
        d->m_routeRequest->setRoutingProfile( d->m_routingManager->profilesModel()->profiles().at( index ) );
    }
}

void RoutingWidget::updateProgress()
{
    if ( !d->m_progressAnimation.isEmpty() ) {
        d->m_currentFrame = ( d->m_currentFrame + 1 ) % d->m_progressAnimation.size();
        QIcon frame = d->m_progressAnimation[d->m_currentFrame];
        d->m_ui.searchButton->setIcon( frame );
        d->m_ui.resultLabel->setVisible( false );
    }
}

void RoutingWidget::switchRoute( int index )
{
    if ( index >= 0 )
    {
        Q_ASSERT( index < d->m_ui.routeComboBox->count() );
        d->m_routingManager->alternativeRoutesModel()->setCurrentRoute( index );
    }
}

void RoutingWidget::updateAlternativeRoutes()
{
    if ( d->m_ui.routeComboBox->count() == 1) {
        // Parts of the route may lie outside the route trip points
        GeoDataLatLonBox const bbox = d->m_routingManager->routingModel()->route().bounds();
        if ( !bbox.isEmpty() ) {
            if ( d->m_zoomRouteAfterDownload ) {
                d->m_zoomRouteAfterDownload = false;
                d->m_widget->centerOn( bbox );
            }
        }
    }

    d->m_ui.routeComboBox->setVisible( d->m_ui.routeComboBox->count() > 0 );
    if ( d->m_ui.routeComboBox->currentIndex() < 0 && d->m_ui.routeComboBox->count() > 0 ) {
        d->m_ui.routeComboBox->setCurrentIndex( 0 );
    }

    d->m_progressTimer.stop();
    d->m_ui.searchButton->setIcon( QIcon() );

    QString const results = tr( "%n routes found", "", d->m_ui.routeComboBox->count() );
    d->m_ui.resultLabel->setText( results );
    d->m_ui.resultLabel->setVisible( true );
    d->m_ui.saveRouteButton->setEnabled( d->m_routingManager->routingModel()->rowCount() > 0 );
}

void RoutingWidget::setShowDirectionsButtonVisible( bool visible )
{
    d->m_ui.showInstructionsButton->setVisible( visible );
}

void RoutingWidget::openRoute()
{
    QString const file = QFileDialog::getOpenFileName( this, tr( "Open Route" ),
                            d->m_routingManager->lastOpenPath(), tr("KML Files (*.kml)") );
    if ( !file.isEmpty() ) {
        d->m_routingManager->setLastOpenPath( QFileInfo( file ).absolutePath() );
        d->m_zoomRouteAfterDownload = true;
        d->m_routingManager->loadRoute( file );
        updateAlternativeRoutes();
    }
}

void RoutingWidget::selectFirstProfile()
{
    int count = d->m_routingManager->profilesModel()->rowCount();
    if ( count && d->m_ui.routingProfileComboBox->currentIndex() < 0 ) {
        d->m_ui.routingProfileComboBox->setCurrentIndex( 0 );
    }
}

void RoutingWidget::setRoutingProfile( int index )
{
    if ( index >= 0 && index < d->m_routingManager->profilesModel()->rowCount() ) {
        d->m_routeRequest->setRoutingProfile( d->m_routingManager->profilesModel()->profiles().at( index ) );
    }
}

void RoutingWidget::showDirections()
{
    d->m_ui.directionsListView->setVisible( true );
}

void RoutingWidget::saveRoute()
{
    QString const fileName = QFileDialog::getSaveFileName( this,
                       tr( "Save Route" ), // krazy:exclude=qclasses
                       d->m_routingManager->lastSavePath(),
                       tr( "KML files (*.kml)" ) );

    if ( !fileName.isEmpty() ) {
        d->m_routingManager->setLastSavePath( QFileInfo( fileName ).absolutePath() );
        d->m_routingManager->saveRoute( fileName );
    }
}

void RoutingWidget::indicateRoutingFailure( GeoDataDocument* route )
{
    if ( !route ) {
        d->m_progressTimer.stop();
        d->m_ui.searchButton->setIcon( QIcon() );
        QString const results = tr( "No route found" );
        d->m_ui.resultLabel->setText( "<font color=\"red\">" + results + "</font>" );
        d->m_ui.resultLabel->setVisible( true );
    }
}

void RoutingWidget::updateActiveRoutingProfile()
{
    RoutingProfile const profile = d->m_routingManager->routeRequest()->routingProfile();
    QList<RoutingProfile> const profiles = d->m_routingManager->profilesModel()->profiles();
    d->m_ui.routingProfileComboBox->setCurrentIndex( profiles.indexOf( profile ) );
}

void RoutingWidget::resizeEvent(QResizeEvent*)
{
  if ( d->m_collapse_width == 0 )
  {
      d->m_collapse_width = d->m_ui.addViaButton->sizeHint().width()
                          + d->m_ui.reverseRouteButton->sizeHint().width()
                          + d->m_ui.clearRouteButton->sizeHint().width()
                          + 20;
  }
  if ( size().width() < d->m_collapse_width )
  {
    d->m_ui.addViaButton->setText("");
    d->m_ui.addViaButton->setIcon(QIcon(":/marble/list-add.png"));
    d->m_ui.reverseRouteButton->setText("");
    d->m_ui.reverseRouteButton->setIcon(QIcon(":/marble/reverse.png"));
    d->m_ui.clearRouteButton->setText("");
    d->m_ui.clearRouteButton->setIcon(QIcon(":/marble/edit-clear.png"));
  }
  else if ( size().width() > d->m_collapse_width + 10 )
  {
    d->m_ui.addViaButton->setText(tr("Add Via"));
    d->m_ui.addViaButton->setIcon(QIcon());
    d->m_ui.reverseRouteButton->setText(tr("Reverse"));
    d->m_ui.reverseRouteButton->setIcon(QIcon());
    d->m_ui.clearRouteButton->setText(tr("Clear"));
    d->m_ui.clearRouteButton->setIcon(QIcon());
  }
}

} // namespace Marble

#include "RoutingWidget.moc"
