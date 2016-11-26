//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010      Dennis Nienhüser <nienhueser@kde.org>
//

#include "RoutingWidget.h"

#include "GeoDataLineString.h"
#include "GeoDataLookAt.h"
#include "GeoDataPlaylist.h"
#include "GeoDataTour.h"
#include "GeoDataFlyTo.h"
#include "GeoDataStyle.h"
#include "GeoDataIconStyle.h"
#include "GeoDataPlacemark.h"
#include "TourPlayback.h"
#include "Maneuver.h"
#include "MarbleModel.h"
#include "MarblePlacemarkModel.h"
#include "MarbleWidget.h"
#include "MarbleWidgetInputHandler.h"
#include "Route.h"
#include "RouteRequest.h"
#include "RoutingInputWidget.h"
#include "RoutingLayer.h"
#include "RoutingManager.h"
#include "RoutingModel.h"
#include "RoutingProfilesModel.h"
#include "RoutingProfileSettingsDialog.h"
#include "GeoDataDocument.h"
#include "GeoDataTreeModel.h"
#include "GeoDataTypes.h"
#include "GeoDataCreate.h"
#include "GeoDataUpdate.h"
#include "GeoDataDelete.h"
#include "AlternativeRoutesModel.h"
#include "RouteSyncManager.h"
#include "CloudRoutesDialog.h"
#include "CloudSyncManager.h"
#include "PlaybackAnimatedUpdateItem.h"
#include "GeoDataAnimatedUpdate.h"
#include "MarbleMath.h"
#include "Planet.h"

#include <QTimer>
#include <QPainter>
#include <QFileDialog>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QToolBar>
#include <QToolButton>
#include <QProgressDialog>

#include "ui_RoutingWidget.h"

namespace Marble
{

struct WaypointInfo
{
    int index;
    double distance; // distance to route start
    GeoDataCoordinates coordinates;
    Maneuver maneuver;
    QString info;

    WaypointInfo( int index_, double distance_, const GeoDataCoordinates &coordinates_, Maneuver maneuver_, const QString& info_ ) :
        index( index_ ),
        distance( distance_ ),
        coordinates( coordinates_ ),
        maneuver( maneuver_ ),
        info( info_ )
    {
        // nothing to do
    }
};

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
    RouteSyncManager *m_routeSyncManager;
    bool m_zoomRouteAfterDownload;
    QTimer m_progressTimer;
    QVector<QIcon> m_progressAnimation;
    GeoDataDocument *m_document;
    GeoDataTour *m_tour;
    TourPlayback *m_playback;
    int m_currentFrame;
    int m_iconSize;
    int m_collapse_width;
    bool m_playing;
    QString m_planetId;

    QToolBar *m_toolBar;

    QToolButton *m_openRouteButton;
    QToolButton *m_saveRouteButton;
    QAction *m_cloudSyncSeparator;
    QAction *m_uploadToCloudAction;
    QAction *m_openCloudRoutesAction;
    QToolButton *m_addViaButton;
    QToolButton *m_reverseRouteButton;
    QToolButton *m_clearRouteButton;
    QToolButton *m_configureButton;
    QToolButton *m_playButton;

    QProgressDialog* m_routeUploadDialog;

    /** Constructor */
    RoutingWidgetPrivate(RoutingWidget *parent, MarbleWidget *marbleWidget );

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

    void setupToolBar();

private:
    void createProgressAnimation();
    RoutingWidget *m_parent;
};

RoutingWidgetPrivate::RoutingWidgetPrivate( RoutingWidget *parent, MarbleWidget *marbleWidget ) :
        m_widget( marbleWidget ),
        m_routingManager( marbleWidget->model()->routingManager() ),
        m_routingLayer( marbleWidget->routingLayer() ),
        m_activeInput( 0 ),
        m_inputRequest( 0 ),
        m_routingModel( m_routingManager->routingModel() ),
        m_routeRequest( marbleWidget->model()->routingManager()->routeRequest() ),
        m_routeSyncManager( 0 ),
        m_zoomRouteAfterDownload( false ),
        m_document( 0 ),
        m_tour( 0 ),
        m_playback( 0 ),
        m_currentFrame( 0 ),
        m_iconSize( 16 ),
        m_collapse_width( 0 ),
        m_playing( false ),
        m_planetId(marbleWidget->model()->planetId()),
        m_toolBar( 0 ),
        m_openRouteButton( 0 ),
        m_saveRouteButton( 0 ),
        m_cloudSyncSeparator( 0 ),
        m_uploadToCloudAction( 0 ),
        m_openCloudRoutesAction( 0 ),
        m_addViaButton( 0 ),
        m_reverseRouteButton( 0 ),
        m_clearRouteButton( 0 ),
        m_configureButton( 0 ),
        m_routeUploadDialog( 0 ),
        m_parent( parent )
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

void RoutingWidgetPrivate::setupToolBar()
{
    m_toolBar = new QToolBar;

    m_openRouteButton = new QToolButton;
    m_openRouteButton->setToolTip( QObject::tr("Open Route") );
    m_openRouteButton->setIcon(QIcon(QStringLiteral(":/icons/16x16/document-open.png")));
    m_toolBar->addWidget(m_openRouteButton);

    m_saveRouteButton = new QToolButton;
    m_saveRouteButton->setToolTip( QObject::tr("Save Route") );
    m_saveRouteButton->setIcon(QIcon(QStringLiteral(":/icons/16x16/document-save.png")));
    m_toolBar->addWidget(m_saveRouteButton);

    m_playButton = new QToolButton;
    m_playButton->setToolTip( QObject::tr("Preview Route") );
    m_playButton->setIcon(QIcon(QStringLiteral(":/marble/playback-play.png")));
    m_toolBar->addWidget(m_playButton);

    m_cloudSyncSeparator = m_toolBar->addSeparator();
    m_uploadToCloudAction = m_toolBar->addAction( QObject::tr("Upload to Cloud") );
    m_uploadToCloudAction->setToolTip( QObject::tr("Upload to Cloud") );
    m_uploadToCloudAction->setIcon(QIcon(QStringLiteral(":/icons/cloud-upload.png")));

    m_openCloudRoutesAction = m_toolBar->addAction( QObject::tr("Manage Cloud Routes") );
    m_openCloudRoutesAction->setToolTip( QObject::tr("Manage Cloud Routes") );
    m_openCloudRoutesAction->setIcon(QIcon(QStringLiteral(":/icons/cloud-download.png")));

    m_toolBar->addSeparator();
    m_addViaButton = new QToolButton;
    m_addViaButton->setToolTip( QObject::tr("Add Via") );
    m_addViaButton->setIcon(QIcon(QStringLiteral(":/marble/list-add.png")));
    m_toolBar->addWidget(m_addViaButton);

    m_reverseRouteButton = new QToolButton;
    m_reverseRouteButton->setToolTip( QObject::tr("Reverse Route") );
    m_reverseRouteButton->setIcon(QIcon(QStringLiteral(":/marble/reverse.png")));
    m_toolBar->addWidget(m_reverseRouteButton);

    m_clearRouteButton = new QToolButton;
    m_clearRouteButton->setToolTip( QObject::tr("Clear Route") );
    m_clearRouteButton->setIcon(QIcon(QStringLiteral(":/marble/edit-clear.png")));
    m_toolBar->addWidget(m_clearRouteButton);

    m_toolBar->addSeparator();

    m_configureButton = new QToolButton;
    m_configureButton->setToolTip( QObject::tr("Settings") );
    m_configureButton->setIcon(QIcon(QStringLiteral(":/icons/16x16/configure.png")));
    m_toolBar->addWidget(m_configureButton);

    QObject::connect( m_openRouteButton, SIGNAL(clicked()),
                      m_parent, SLOT(openRoute()) );
    QObject::connect( m_saveRouteButton, SIGNAL(clicked()),
                      m_parent, SLOT(saveRoute()) );
    QObject::connect( m_uploadToCloudAction, SIGNAL(triggered()),
                      m_parent, SLOT(uploadToCloud()) );
    QObject::connect( m_openCloudRoutesAction, SIGNAL(triggered()),
                      m_parent, SLOT(openCloudRoutesDialog()));
    QObject::connect( m_addViaButton, SIGNAL(clicked()),
                      m_parent, SLOT(addInputWidget()) );
    QObject::connect( m_reverseRouteButton, SIGNAL(clicked()),
                      m_routingManager, SLOT(reverseRoute()) );
    QObject::connect( m_clearRouteButton, SIGNAL(clicked()),
                      m_routingManager, SLOT(clearRoute()) );
    QObject::connect( m_configureButton, SIGNAL(clicked()),
                      m_parent,  SLOT(configureProfile()) );
    QObject::connect( m_playButton, SIGNAL(clicked()),
                      m_parent,  SLOT(toggleRoutePlay()) );

    m_toolBar->setIconSize(QSize(16, 16));
    m_ui.toolBarLayout->addWidget(m_toolBar, 0, Qt::AlignLeft);
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
    QWidget( parent ), d( new RoutingWidgetPrivate( this, marbleWidget ) )
{
    d->m_ui.setupUi( this );
    d->setupToolBar();
    d->m_ui.routeComboBox->setVisible( false );
    d->m_ui.routeComboBox->setModel( d->m_routingManager->alternativeRoutesModel() );
    layout()->setMargin( 0 );

    d->m_ui.routingProfileComboBox->setModel( d->m_routingManager->profilesModel() );

    connect( d->m_routingManager->profilesModel(), SIGNAL(rowsInserted(QModelIndex,int,int)),
             this, SLOT(selectFirstProfile()) );
    connect( d->m_routingManager->profilesModel(), SIGNAL(modelReset()),
             this, SLOT(selectFirstProfile()) );
    connect( d->m_routingLayer, SIGNAL(placemarkSelected(QModelIndex)),
             this, SLOT(activatePlacemark(QModelIndex)) );
    connect( d->m_routingManager, SIGNAL(stateChanged(RoutingManager::State)),
             this, SLOT(updateRouteState(RoutingManager::State)) );
    connect( d->m_routeRequest, SIGNAL(positionAdded(int)),
             this, SLOT(insertInputWidget(int)) );
    connect( d->m_routeRequest, SIGNAL(positionRemoved(int)),
             this, SLOT(removeInputWidget(int)) );
    connect( d->m_routeRequest, SIGNAL(routingProfileChanged()),
             this, SLOT(updateActiveRoutingProfile()) );
    connect( &d->m_progressTimer, SIGNAL(timeout()),
             this, SLOT(updateProgress()) );
    connect( d->m_ui.routeComboBox, SIGNAL(currentIndexChanged(int)),
             d->m_routingManager->alternativeRoutesModel(), SLOT(setCurrentRoute(int)) );
    connect( d->m_routingManager->alternativeRoutesModel(), SIGNAL(currentRouteChanged(int)),
             d->m_ui.routeComboBox, SLOT(setCurrentIndex(int)) );
    connect( d->m_ui.routingProfileComboBox, SIGNAL(currentIndexChanged(int)),
             this, SLOT(setRoutingProfile(int)) );
    connect( d->m_ui.routingProfileComboBox, SIGNAL(activated(int)),
             this, SLOT(retrieveRoute()) );
    connect( d->m_routingManager->alternativeRoutesModel(), SIGNAL(rowsInserted(QModelIndex,int,int)),
             this, SLOT(updateAlternativeRoutes()) );

    d->m_ui.directionsListView->setModel( d->m_routingModel );

    QItemSelectionModel *selectionModel = d->m_ui.directionsListView->selectionModel();
    d->m_routingLayer->synchronizeWith( selectionModel );
    connect( d->m_ui.directionsListView, SIGNAL(activated(QModelIndex)),
             this, SLOT(activateItem(QModelIndex)) );

    // FIXME: apply for this sector
    connect( d->m_ui.searchButton, SIGNAL(clicked()),
             this, SLOT(retrieveRoute()) );
    connect( d->m_ui.showInstructionsButton, SIGNAL(clicked(bool)),
             this, SLOT(showDirections()) );

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
    updateCloudSyncButtons();

    if ( MarbleGlobal::getInstance()->profiles() & MarbleGlobal::SmallScreen ) {
        d->m_ui.directionsListView->setVisible( false );
        d->m_openRouteButton->setVisible( false );
        d->m_saveRouteButton->setVisible( false );
    }

    connect( marbleWidget->model(), SIGNAL(themeChanged(QString)),
             this, SLOT(handlePlanetChange()) );
}

RoutingWidget::~RoutingWidget()
{
    delete d->m_playback;
    delete d->m_tour;
    if( d->m_document ){
        d->m_widget->model()->treeModel()->removeDocument( d->m_document );
        delete d->m_document;
    }
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

    if( d->m_playback ) {
        d->m_playback->stop();
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
            d->m_activeInput->setTargetPosition( data.value<GeoDataCoordinates>(), index.data().toString() );
        }
    }
}

void RoutingWidget::handleSearchResult( RoutingInputWidget *widget )
{
    d->setActiveInput( widget );
    MarblePlacemarkModel *model = widget->searchResultModel();

    if ( model->rowCount() ) {
        QString const results = tr( "placemarks found: %1" ).arg( model->rowCount() );
        d->m_ui.resultLabel->setText( results );
        d->m_ui.resultLabel->setVisible( true );
        // Make sure we have a selection
        activatePlacemark( model->index( 0, 0 ) );
    } else {
        QString const results = tr( "No placemark found" );
        d->m_ui.resultLabel->setText(QLatin1String("<font color=\"red\">") + results + QLatin1String("</font>"));
        d->m_ui.resultLabel->setVisible( true );
    }

    GeoDataLineString placemarks;
    for ( int i = 0; i < model->rowCount(); ++i ) {
        QVariant data = model->index( i, 0 ).data( MarblePlacemarkModel::CoordinateRole );
        if ( !data.isNull() ) {
            placemarks << data.value<GeoDataCoordinates>();
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
            d->m_activeInput->setTargetPosition( data.value<GeoDataCoordinates>() );
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
        RoutingInputWidget *input = new RoutingInputWidget( d->m_widget->model(), index, this );
        d->m_inputWidgets.insert( index, input );
        connect( input, SIGNAL(searchFinished(RoutingInputWidget*)),
                 this, SLOT(handleSearchResult(RoutingInputWidget*)) );
        connect( input, SIGNAL(removalRequest(RoutingInputWidget*)),
                 this, SLOT(removeInputWidget(RoutingInputWidget*)) );
        connect( input, SIGNAL(activityRequest(RoutingInputWidget*)),
                 this, SLOT(centerOnInputWidget(RoutingInputWidget*)) );
        connect( input, SIGNAL(mapInputModeEnabled(RoutingInputWidget*,bool)),
                 this, SLOT(requestMapPosition(RoutingInputWidget*,bool)) );
        connect( input, SIGNAL(targetValidityChanged(bool)),
                 this, SLOT(adjustSearchButton()) );

        d->m_ui.inputLayout->insertWidget( index, input );
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
        d->m_ui.inputLayout->removeWidget( widget );
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
    clearTour();

    switch ( state ) {
    case RoutingManager::Downloading:
        d->m_ui.routeComboBox->setVisible( false );
        d->m_ui.routeComboBox->clear();
        d->m_progressTimer.start();
        d->m_ui.resultLabel->setVisible( false );
    break;
    case RoutingManager::Retrieved: {
        d->m_progressTimer.stop();
        d->m_ui.searchButton->setIcon( QIcon() );
        if ( d->m_routingManager->routingModel()->rowCount() == 0 ) {
            const QString results = tr( "No route found" );
            d->m_ui.resultLabel->setText(QLatin1String("<font color=\"red\">") + results + QLatin1String("</font>"));
            d->m_ui.resultLabel->setVisible( true );
        }
    }
    break;
    }

    d->m_saveRouteButton->setEnabled( d->m_routingManager->routingModel()->rowCount() > 0 );
}

void RoutingWidget::requestMapPosition( RoutingInputWidget *widget, bool enabled )
{
    pointSelectionCanceled();

    if ( enabled ) {
        d->m_inputRequest = widget;
        d->m_widget->installEventFilter( this );
        d->m_widget->setFocus( Qt::OtherFocusReason );
    }
}

void RoutingWidget::retrieveSelectedPoint( const GeoDataCoordinates &coordinates )
{
    if ( d->m_inputRequest && d->m_inputWidgets.contains( d->m_inputRequest ) ) {
        d->m_inputRequest->setTargetPosition( coordinates );
        d->m_widget->update();
    }

    d->m_inputRequest = 0;
    d->m_widget->removeEventFilter( this );
}

void RoutingWidget::adjustSearchButton()
{
    d->adjustSearchButton();
}

void RoutingWidget::pointSelectionCanceled()
{
    if ( d->m_inputRequest && d->m_inputWidgets.contains( d->m_inputRequest ) ) {
        d->m_inputRequest->abortMapInputRequest();
    }

    d->m_inputRequest = 0;
    d->m_widget->removeEventFilter( this );
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
    }
}

void RoutingWidget::updateAlternativeRoutes()
{
    if ( d->m_ui.routeComboBox->count() == 1) {
        // Parts of the route may lie outside the route trip points
        GeoDataLatLonBox const bbox = d->m_routingManager->routingModel()->route().bounds();
        if ( d->m_zoomRouteAfterDownload ) {
            d->m_zoomRouteAfterDownload = false;
            d->m_widget->centerOn( bbox );
        }
    }

    d->m_ui.routeComboBox->setVisible( d->m_ui.routeComboBox->count() > 0 );
    if ( d->m_ui.routeComboBox->currentIndex() < 0 && d->m_ui.routeComboBox->count() > 0 ) {
        d->m_ui.routeComboBox->setCurrentIndex( 0 );
    }

    QString const results = tr( "routes found: %1" ).arg( d->m_ui.routeComboBox->count() );
    d->m_ui.resultLabel->setText( results );
    d->m_ui.resultLabel->setVisible( true );
    d->m_saveRouteButton->setEnabled( d->m_routingManager->routingModel()->rowCount() > 0 );
}

void RoutingWidget::setShowDirectionsButtonVisible( bool visible )
{
    d->m_ui.showInstructionsButton->setVisible( visible );
}

void RoutingWidget::setRouteSyncManager(RouteSyncManager *manager)
{
    d->m_routeSyncManager = manager;
    connect( d->m_routeSyncManager, SIGNAL(routeSyncEnabledChanged(bool)),
             this, SLOT(updateCloudSyncButtons()) );
    updateCloudSyncButtons();
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
    QString fileName = QFileDialog::getSaveFileName( this,
                       tr( "Save Route" ), // krazy:exclude=qclasses
                       d->m_routingManager->lastSavePath(),
                       tr( "KML files (*.kml)" ) );

    if ( !fileName.isEmpty() ) {
        // maemo 5 file dialog does not append the file extension
        if ( !fileName.endsWith(QLatin1String( ".kml" ), Qt::CaseInsensitive) ) {
            fileName += QLatin1String(".kml");
        }
        d->m_routingManager->setLastSavePath( QFileInfo( fileName ).absolutePath() );
        d->m_routingManager->saveRoute( fileName );
    }
}

void RoutingWidget::uploadToCloud()
{
    Q_ASSERT( d->m_routeSyncManager );

    if (!d->m_routeUploadDialog) {
        d->m_routeUploadDialog = new QProgressDialog( d->m_widget );
        d->m_routeUploadDialog->setWindowTitle( tr( "Uploading route..." ) );
        d->m_routeUploadDialog->setMinimum( 0 );
        d->m_routeUploadDialog->setMaximum( 100 );
        d->m_routeUploadDialog->setAutoClose( true );
        d->m_routeUploadDialog->setAutoReset( true );
        connect( d->m_routeSyncManager, SIGNAL(routeUploadProgress(qint64,qint64)), this, SLOT(updateUploadProgress(qint64,qint64)) );
    }

    d->m_routeUploadDialog->show();
    d->m_routeSyncManager->uploadRoute();
}

void RoutingWidget::openCloudRoutesDialog()
{
    Q_ASSERT( d->m_routeSyncManager );
    d->m_routeSyncManager->prepareRouteList();

    QPointer<CloudRoutesDialog> dialog = new CloudRoutesDialog( d->m_routeSyncManager->model(), d->m_widget );
    connect( d->m_routeSyncManager, SIGNAL(routeListDownloadProgress(qint64,qint64)), dialog, SLOT(updateListDownloadProgressbar(qint64,qint64)) );
    connect( dialog, SIGNAL(downloadButtonClicked(QString)), d->m_routeSyncManager, SLOT(downloadRoute(QString)) );
    connect( dialog, SIGNAL(openButtonClicked(QString)), this, SLOT(openCloudRoute(QString)) );
    connect( dialog, SIGNAL(deleteButtonClicked(QString)), d->m_routeSyncManager, SLOT(deleteRoute(QString)) );
    connect( dialog, SIGNAL(removeFromCacheButtonClicked(QString)), d->m_routeSyncManager, SLOT(removeRouteFromCache(QString)) );
    connect( dialog, SIGNAL(uploadToCloudButtonClicked(QString)), d->m_routeSyncManager, SLOT(uploadRoute(QString)) );
    dialog->exec();
    delete dialog;
}

void RoutingWidget::updateActiveRoutingProfile()
{
    RoutingProfile const profile = d->m_routingManager->routeRequest()->routingProfile();
    QList<RoutingProfile> const profiles = d->m_routingManager->profilesModel()->profiles();
    d->m_ui.routingProfileComboBox->setCurrentIndex( profiles.indexOf( profile ) );
}

void RoutingWidget::updateCloudSyncButtons()
{
    bool const show = d->m_routeSyncManager && d->m_routeSyncManager->isRouteSyncEnabled();
    d->m_cloudSyncSeparator->setVisible( show );
    d->m_uploadToCloudAction->setVisible( show );
    d->m_openCloudRoutesAction->setVisible( show );
}

void RoutingWidget::openCloudRoute(const QString &identifier)
{
    Q_ASSERT( d->m_routeSyncManager );
    d->m_routeSyncManager->openRoute( identifier );
    d->m_widget->centerOn( d->m_routingManager->routingModel()->route().bounds() );
}

void RoutingWidget::updateUploadProgress(qint64 sent, qint64 total)
{
    Q_ASSERT( d->m_routeUploadDialog );
    d->m_routeUploadDialog->setValue( 100.0 * sent / total );
}

bool RoutingWidget::eventFilter( QObject *o, QEvent *event )
{
    if ( o != d->m_widget ) {
        return QWidget::eventFilter( o, event );
    }

    Q_ASSERT( d->m_inputRequest != 0 );
    Q_ASSERT( d->m_inputWidgets.contains( d->m_inputRequest ) );

    if ( event->type() == QEvent::MouseButtonPress ) {
        QMouseEvent *e = static_cast<QMouseEvent*>( event );
        return e->button() == Qt::LeftButton;
    }

    if ( event->type() == QEvent::MouseButtonRelease ) {
        QMouseEvent *e = static_cast<QMouseEvent*>( event );
        qreal lon( 0.0 ), lat( 0.0 );
        if ( e->button() == Qt::LeftButton && d->m_widget->geoCoordinates( e->pos().x(), e->pos().y(),
                                                                                 lon, lat, GeoDataCoordinates::Radian ) ) {
            retrieveSelectedPoint( GeoDataCoordinates( lon, lat ) );
            return true;
        } else {
            return QWidget::eventFilter( o, event );
        }
    }

    if ( event->type() == QEvent::MouseMove ) {
        d->m_widget->setCursor( Qt::CrossCursor );
        return true;
    }

    if ( event->type() == QEvent::KeyPress ) {
        QKeyEvent *e = static_cast<QKeyEvent*>( event );
        if ( e->key() == Qt::Key_Escape ) {
            pointSelectionCanceled();
            return true;
        }

        return QWidget::eventFilter( o, event );
    }

    return QWidget::eventFilter( o, event );
}

void RoutingWidget::resizeEvent(QResizeEvent *e)
{
    QWidget::resizeEvent(e);
}

void RoutingWidget::toggleRoutePlay()
{
    if( !d->m_playback ){
        if( d->m_routingModel->rowCount() != 0 ){
            initializeTour();
	}
    }

    if (!d->m_playback)
        return;

    if( !d->m_playing ){
        d->m_playing = true;
        d->m_playButton->setIcon(QIcon(QStringLiteral(":/marble/playback-pause.png")));

        if( d->m_playback ){
            d->m_playback->play();
        }
    } else {
        d->m_playing = false;
        d->m_playButton->setIcon(QIcon(QStringLiteral(":/marble/playback-play.png")));
        d->m_playback->pause();
    }
}

void RoutingWidget::initializeTour()
{
    d->m_tour = new GeoDataTour;
    if( d->m_document ){
        d->m_widget->model()->treeModel()->removeDocument( d->m_document );
        delete d->m_document;
    }
    d->m_document = new GeoDataDocument;
    d->m_document->setId(QStringLiteral("tourdoc"));
    d->m_document->append( d->m_tour );

    d->m_tour->setPlaylist( new GeoDataPlaylist );
    Route const route = d->m_widget->model()->routingManager()->routingModel()->route();
    GeoDataLineString path = route.path();
    if ( path.size() < 1 ){
        return;
    }

    QList<WaypointInfo> waypoints;
    double totalDistance = 0.0;
    for( int i=0; i<route.size(); ++i ){
        // TODO: QString( i )?
        waypoints << WaypointInfo(i, totalDistance, route.at(i).path().first(), route.at(i).maneuver(), QLatin1String("start ") + QString(i));
        totalDistance += route.at( i ).distance();
    }

    if( waypoints.size() < 1 ){
        return;
    }

    QList<WaypointInfo> const allWaypoints = waypoints;
    totalDistance = 0.0;
    GeoDataCoordinates last = path.at( 0 );
    int j=0; // next waypoint
    qreal planetRadius = d->m_widget->model()->planet()->radius();
    for( int i=1; i<path.size(); ++i ){
        GeoDataCoordinates coordinates = path.at( i );
        totalDistance += planetRadius * distanceSphere( path.at( i-1 ), coordinates ); // Distance to route start
        while (totalDistance >= allWaypoints[j].distance && j+1<allWaypoints.size()) {
            ++j;
        }
        int const lastIndex = qBound( 0, j-1, allWaypoints.size()-1 ); // previous waypoint
        double const lastDistance = qAbs( totalDistance - allWaypoints[lastIndex].distance );
        double const nextDistance = qAbs( allWaypoints[j].distance - totalDistance );
        double const waypointDistance = qMin( lastDistance, nextDistance ); // distance to closest waypoint
        double const step = qBound( 100.0, waypointDistance*2, 1000.0 ); // support point distance (higher density close to waypoints)

        double const distance = planetRadius * distanceSphere( last, coordinates );
        if( i > 1 && distance < step ){
            continue;
        }
        last = coordinates;

        GeoDataLookAt* lookat = new GeoDataLookAt;
        // Choose a zoom distance of 400, 600 or 800 meters based on the distance to the closest waypoint
        double const range = waypointDistance < 400 ? 400 : ( waypointDistance < 2000 ? 600 : 800 );
        coordinates.setAltitude( range );
        lookat->setCoordinates( coordinates );
        lookat->setRange( range );
        GeoDataFlyTo* flyto = new GeoDataFlyTo;
        double const duration = 0.75;
        flyto->setDuration( duration );
        flyto->setView( lookat );
        flyto->setFlyToMode( GeoDataFlyTo::Smooth );
        d->m_tour->playlist()->addPrimitive( flyto );

        if( !waypoints.empty() && totalDistance > waypoints.first().distance-100 ){
            WaypointInfo const waypoint = waypoints.first();
            waypoints.pop_front();
            GeoDataAnimatedUpdate *updateCreate = new GeoDataAnimatedUpdate;
            updateCreate->setUpdate( new GeoDataUpdate );
            updateCreate->update()->setCreate( new GeoDataCreate );
            GeoDataPlacemark *placemarkCreate = new GeoDataPlacemark;
            QString const waypointId = QString( "waypoint-%1" ).arg( i, 0, 10 );
            placemarkCreate->setId( waypointId );
            placemarkCreate->setTargetId( d->m_document->id() );
            placemarkCreate->setCoordinate( waypoint.coordinates );
            GeoDataStyle::Ptr style(new GeoDataStyle);
            style->iconStyle().setIconPath( waypoint.maneuver.directionPixmap() );
            placemarkCreate->setStyle( style );
            updateCreate->update()->create()->append( placemarkCreate );
            d->m_tour->playlist()->addPrimitive( updateCreate );

            GeoDataAnimatedUpdate *updateDelete = new GeoDataAnimatedUpdate;
            updateDelete->setDelayedStart( 2 );
            updateDelete->setUpdate( new GeoDataUpdate );
            updateDelete->update()->setDelete( new GeoDataDelete );
            GeoDataPlacemark *placemarkDelete = new GeoDataPlacemark;
            placemarkDelete->setTargetId( waypointId );
            updateDelete->update()->getDelete()->append( placemarkDelete );
            d->m_tour->playlist()->addPrimitive( updateDelete );
        }
    }

    d->m_playback = new TourPlayback;
    d->m_playback->setMarbleWidget( d->m_widget );
    d->m_playback->setTour( d->m_tour );
    d->m_widget->model()->treeModel()->addDocument( d->m_document );
    QObject::connect( d->m_playback, SIGNAL(finished()),
                  this, SLOT(seekTourToStart()) );
}

void RoutingWidget::centerOn( const GeoDataCoordinates &coordinates )
{
    if ( d->m_widget ) {
        GeoDataLookAt lookat;
        lookat.setCoordinates( coordinates );
        lookat.setRange( coordinates.altitude() );
        d->m_widget->flyTo( lookat, Instant );
    }
}

void RoutingWidget::clearTour()
{
    d->m_playing = false;
    d->m_playButton->setIcon(QIcon(QStringLiteral(":/marble/playback-play.png")));
    delete d->m_playback;
    d->m_playback = 0;
    if( d->m_document ){
        d->m_widget->model()->treeModel()->removeDocument( d->m_document );
        delete d->m_document;
        d->m_document = 0;
        d->m_tour = 0;
    }
}

void RoutingWidget::seekTourToStart()
{
    Q_ASSERT( d->m_playback );
    d->m_playback->stop();
    d->m_playback->seek( 0 );
    d->m_playButton->setIcon(QIcon(QStringLiteral(":/marble/playback-play.png")));
    d->m_playing = false;
}

void RoutingWidget::handlePlanetChange()
{
    const QString newPlanetId = d->m_widget->model()->planetId();

    if (newPlanetId == d->m_planetId) {
        return;
    }

    d->m_planetId = newPlanetId;
    d->m_routingManager->clearRoute();
}

} // namespace Marble

#include "moc_RoutingWidget.cpp"
