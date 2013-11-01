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
#include "RouteSyncManager.h"
#include "CloudRoutesDialog.h"
#include "CloudSyncManager.h"

#include <QTime>
#include <QTimer>
#include <QSortFilterProxyModel>
#include <QComboBox>
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

    int m_currentFrame;

    int m_iconSize;

    int m_collapse_width;

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
        m_currentFrame( 0 ),
        m_iconSize( 16 ),
        m_collapse_width( 0 ),
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
    m_openRouteButton->setIcon( QIcon(":/icons/16x16/document-open.png") );
    m_toolBar->addWidget(m_openRouteButton);

    m_saveRouteButton = new QToolButton;
    m_saveRouteButton->setToolTip( QObject::tr("Save Route") );
    m_saveRouteButton->setIcon( QIcon(":/icons/16x16/document-save.png") );
    m_toolBar->addWidget(m_saveRouteButton);

    m_cloudSyncSeparator = m_toolBar->addSeparator();
    m_uploadToCloudAction = m_toolBar->addAction( QObject::tr("Upload to Cloud") );
    m_uploadToCloudAction->setToolTip( QObject::tr("Upload to Cloud") );
    m_uploadToCloudAction->setIcon( QIcon(":/icons/cloud-upload.png") );

    m_openCloudRoutesAction = m_toolBar->addAction( QObject::tr("Manage Cloud Routes") );
    m_openCloudRoutesAction->setToolTip( QObject::tr("Manage Cloud Routes") );
    m_openCloudRoutesAction->setIcon( QIcon(":/icons/cloud-download.png") );

    m_toolBar->addSeparator();
    m_addViaButton = new QToolButton;
    m_addViaButton->setToolTip( QObject::tr("Add Via") );
    m_addViaButton->setIcon( QIcon(":/marble/list-add.png") );
    m_toolBar->addWidget(m_addViaButton);

    m_reverseRouteButton = new QToolButton;
    m_reverseRouteButton->setToolTip( QObject::tr("Reverse Route") );
    m_reverseRouteButton->setIcon( QIcon(":/marble/reverse.png") );
    m_toolBar->addWidget(m_reverseRouteButton);

    m_clearRouteButton = new QToolButton;
    m_clearRouteButton->setToolTip( QObject::tr("Clear Route") );
    m_clearRouteButton->setIcon( QIcon(":/marble/edit-clear.png") );
    m_toolBar->addWidget(m_clearRouteButton);

    m_toolBar->addSeparator();

    m_configureButton = new QToolButton;
    m_configureButton->setToolTip( QObject::tr("Settings") );
    m_configureButton->setIcon( QIcon(":/icons/16x16/configure.png") );
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
    connect( d->m_routingManager, SIGNAL(routeRetrieved(GeoDataDocument*)),
             this, SLOT(indicateRoutingFailure(GeoDataDocument*)) );
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
        d->m_ui.resultLabel->setText( "<font color=\"red\">" + results + "</font>" );
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
        RoutingInputWidget *input = new RoutingInputWidget( d->m_widget, index, this );
        input->setProgressAnimation( d->m_progressAnimation );
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
        d->m_ui.resultLabel->setVisible( false );
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

    d->m_progressTimer.stop();
    d->m_ui.searchButton->setIcon( QIcon() );

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
            fileName.append( ".kml" );
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

    CloudRoutesDialog *dialog = new CloudRoutesDialog( d->m_routeSyncManager->model(), d->m_widget );
    connect( d->m_routeSyncManager, SIGNAL(routeListDownloadProgress(qint64,qint64)), dialog, SLOT(updateListDownloadProgressbar(qint64,qint64)) );
    connect( dialog, SIGNAL(downloadButtonClicked(QString)), d->m_routeSyncManager, SLOT(downloadRoute(QString)) );
    connect( dialog, SIGNAL(openButtonClicked(QString)), this, SLOT(openCloudRoute(QString)) );
    connect( dialog, SIGNAL(deleteButtonClicked(QString)), d->m_routeSyncManager, SLOT(deleteRoute(QString)) );
    connect( dialog, SIGNAL(removeFromCacheButtonClicked(QString)), d->m_routeSyncManager, SLOT(removeRouteFromCache(QString)) );
    connect( dialog, SIGNAL(uploadToCloudButtonClicked(QString)), d->m_routeSyncManager, SLOT(uploadRoute(QString)) );
    dialog->exec();
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

} // namespace Marble

#include "RoutingWidget.moc"
