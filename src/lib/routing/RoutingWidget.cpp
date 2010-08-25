//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010      Dennis Nienhüser <earthwings@gentoo.org>
//

#include "RoutingWidget.h"

#include "GeoDataLineString.h"
#include "MarbleMap.h"
#include "MarbleModel.h"
#include "MarblePlacemarkModel.h"
#include "MarbleWidget.h"
#include "MarbleWidgetInputHandler.h"
#include "RouteSkeleton.h"
#include "RoutingInputWidget.h"
#include "RoutingLayer.h"
#include "RoutingManager.h"
#include "RoutingModel.h"
#include "RoutingProxyModel.h"

#include <QtCore/QTime>
#include <QtCore/QTimer>
#include <QtGui/QFileDialog>
#include <QtGui/QSortFilterProxyModel>
#include <QtGui/QPainter>

#include "ui_RoutingWidget.h"

namespace Marble
{

class RoutingWidgetPrivate
{
public:
    Ui::RoutingWidget m_ui;

    MarbleWidget *m_widget;

    RoutingManager *m_routingManager;

    RoutingLayer *m_routingLayer;

    RoutingInputWidget *m_activeInput;

    QVector<RoutingInputWidget*> m_inputWidgets;

    RoutingInputWidget *m_inputRequest;

    QSortFilterProxyModel *m_routingProxyModel;

    RouteSkeleton *m_routeSkeleton;

    bool m_zoomRouteAfterDownload;

    bool m_workOffline;

    QTimer m_progressTimer;

    QVector<QIcon> m_progressAnimation;

    int m_currentFrame;

    /** Constructor */
    RoutingWidgetPrivate();

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

RoutingWidgetPrivate::RoutingWidgetPrivate() :
        m_widget( 0 ), m_routingManager( 0 ), m_routingLayer( 0 ),
        m_activeInput( 0 ), m_inputRequest( 0 ), m_routingProxyModel( 0 ),
        m_routeSkeleton( 0 ), m_zoomRouteAfterDownload( false ),
        m_workOffline( false ), m_currentFrame( 0 )
{
    createProgressAnimation();
    m_progressTimer.setInterval( 100 );
}

void RoutingWidgetPrivate::adjustInputWidgets()
{
    for ( int i = 0; i < m_inputWidgets.size(); ++i ) {
        m_inputWidgets[i]->setIndex( i );
    }

    //m_ui.optionsLabel->setVisible( !simple );
    adjustSearchButton();
}

void RoutingWidgetPrivate::adjustSearchButton()
{
    QString text = QObject::tr( "Get Directions" );
    QString tooltip = QObject::tr( "Retrieve routing instructions for the selected destinations." );

    bool search = m_inputWidgets.size() < 2;
    if ( m_inputWidgets.size() > 1 ) {
        for ( int i = 0; i < m_inputWidgets.size(); ++i ) {
            if ( !m_inputWidgets[i]->hasInput() ||
                    ( m_inputWidgets[i]->hasInput() && !m_inputWidgets[i]->hasTargetPosition() ) ) {
                search = true;
            }
        }
    }

    if ( search ) {
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
    m_routingLayer->setModel( model );
    m_routingLayer->synchronizeWith( m_routingProxyModel, m_ui.directionsListView->selectionModel() );
}

void RoutingWidgetPrivate::createProgressAnimation()
{
    // Size parameters
    int const iconSize = 16;
    qreal const h = iconSize / 2.0; // Half of the icon size
    qreal const q = h / 2.0; // Quarter of the icon size
    qreal const d = 7.5; // Circle diameter
    qreal const r = d / 2.0; // Circle radius

    // Canvas parameters
    QImage canvas( iconSize, iconSize, QImage::Format_ARGB32 );
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
        QWidget( parent ), d( new RoutingWidgetPrivate )
{
    d->m_ui.setupUi( this );
    d->m_widget = marbleWidget;

    d->m_routeSkeleton = new RouteSkeleton( this );
    d->m_routingManager = new RoutingManager( d->m_widget, this );
    d->m_routingLayer = new RoutingLayer( d->m_widget, this );
    d->m_routingLayer->setRouteSkeleton( d->m_routeSkeleton );
    d->m_widget->map()->model()->addLayer( d->m_routingLayer );

    connect( d->m_routingLayer, SIGNAL( routeDirty() ),
             d->m_routingManager, SLOT( updateRoute() ) );
    connect( d->m_routingLayer, SIGNAL( placemarkSelected( QModelIndex ) ),
             this, SLOT( activatePlacemark( QModelIndex ) ) );
    connect( d->m_routingLayer, SIGNAL( pointSelected( GeoDataCoordinates ) ),
             this, SLOT( retrieveSelectedPoint( GeoDataCoordinates ) ) );
    connect( d->m_routingLayer, SIGNAL( pointSelectionAborted() ),
             this, SLOT( pointSelectionCanceled() ) );
    connect( d->m_routingLayer, SIGNAL( exportRequested() ),
             this, SLOT( exportRoute() ) );
    connect( d->m_routingManager, SIGNAL( stateChanged( RoutingManager::State, RouteSkeleton* ) ),
             this, SLOT( updateRouteState( RoutingManager::State, RouteSkeleton* ) ) );
    connect( d->m_routeSkeleton, SIGNAL( positionAdded( int ) ),
             this, SLOT( insertInputWidget( int ) ) );
    connect( d->m_routeSkeleton, SIGNAL( positionRemoved( int ) ),
             this, SLOT( removeInputWidget( int ) ) );
    connect( &d->m_progressTimer, SIGNAL( timeout() ),
             this, SLOT( updateProgress() ) );

    d->m_routingProxyModel = new RoutingProxyModel( this );
    d->m_routingProxyModel->setSourceModel( d->m_routingManager->routingModel() );
    d->m_ui.directionsListView->setModel( d->m_routingProxyModel );

    d->m_routingLayer->setModel( d->m_routingManager->routingModel() );
    QItemSelectionModel *selectionModel = d->m_ui.directionsListView->selectionModel();
    d->m_routingLayer->synchronizeWith( d->m_routingProxyModel, selectionModel );
    connect( d->m_ui.directionsListView, SIGNAL( activated ( QModelIndex ) ),
             this, SLOT( activateItem ( QModelIndex ) ) );

    connect( d->m_ui.searchButton, SIGNAL( clicked( ) ),
             this, SLOT( retrieveRoute () ) );
    connect( d->m_ui.optionsLabel, SIGNAL( linkActivated( QString ) ),
             this, SLOT( toggleOptionsVisibility() ) );

    addInputWidget();
    addInputWidget(); // Start with source and destination
    d->m_ui.routePreferenceComboBox->setVisible( false );
    d->m_ui.highwaysCheckBox->setVisible( false );
    d->m_ui.tollWaysCheckBox->setVisible( false );
    d->m_ui.preferenceLabel->setVisible( false );
    d->m_ui.avoidLabel->setVisible( false );
    d->m_ui.descriptionLabel->setVisible( false );
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

    int index = d->m_ui.routePreferenceComboBox->currentIndex();
    RouteSkeleton::RoutePreference pref = RouteSkeleton::CarFastest;
    if ( index == 1 ) {
        pref = RouteSkeleton::CarShortest;
    }
    if ( index == 2 ) {
        pref = RouteSkeleton::Bicycle;
    }
    if ( index == 3 ) {
        pref = RouteSkeleton::Pedestrian;
    }
    RouteSkeleton::AvoidFeatures avoid = RouteSkeleton::AvoidNone;
    if ( d->m_ui.highwaysCheckBox->isChecked() ) {
        avoid |= RouteSkeleton::AvoidHighway;
    }
    if ( d->m_ui.tollWaysCheckBox->isChecked() ) {
        avoid |= RouteSkeleton::AvoidTollWay;
    }

    d->m_routeSkeleton->setRoutePreference( pref );
    d->m_routeSkeleton->setAvoidFeatures( avoid );

    Q_ASSERT( d->m_routeSkeleton->size() == d->m_inputWidgets.size() );
    for ( int i = 0; i < d->m_inputWidgets.size(); ++i ) {
        RoutingInputWidget *widget = d->m_inputWidgets.at( i );
        if ( !widget->hasTargetPosition() && widget->hasInput() ) {
            widget->findPlacemarks();
            return;
        }
    }

    d->m_activeInput = 0;
    if ( d->m_routeSkeleton->size() > 1 ) {
        d->m_zoomRouteAfterDownload = true;
        d->m_routingLayer->setModel( d->m_routingManager->routingModel() );
        d->m_routingManager->retrieveRoute( d->m_routeSkeleton );
        d->m_ui.directionsListView->setModel( d->m_routingProxyModel );
        d->m_routingLayer->synchronizeWith( d->m_routingProxyModel,
                                            d->m_ui.directionsListView->selectionModel() );
    }
}

void RoutingWidget::activateItem ( const QModelIndex &index )
{
    // Underlying model can be both a placemark model and a routing model
    // We rely on the same role index for coordinates
    Q_ASSERT( int( RoutingModel::CoordinateRole ) == int( MarblePlacemarkModel::CoordinateRole ) );

    QVariant data = index.data( RoutingModel::CoordinateRole );

    if ( !data.isNull() ) {
        GeoDataCoordinates position = qvariant_cast<GeoDataCoordinates>( data );
        d->m_widget->centerOn( position, true );
    }

    if ( d->m_activeInput && index.isValid() ) {
        QVariant data = index.data( MarblePlacemarkModel::CoordinateRole );
        if ( !data.isNull() ) {
            d->m_activeInput->setTargetPosition( qVariantValue<GeoDataCoordinates>( data) );
        }
    }
}

void RoutingWidget::handleSearchResult( RoutingInputWidget *widget )
{
    d->setActiveInput( widget );
    MarblePlacemarkModel *model = widget->searchResultModel();

    if ( model->rowCount() ) {
        // Make sure we have a selection
        activatePlacemark( model->index( 0, 0 ) );
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
        d->m_ui.descriptionLabel->setVisible( false );
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
    int index = d->m_ui.routingLayout->count() - 4;
    d->m_routeSkeleton->append( GeoDataCoordinates() );
    insertInputWidget( index );
}

void RoutingWidget::insertInputWidget( int index )
{
    if ( index >= 0 && index <= d->m_inputWidgets.size() ) {
        RoutingInputWidget *input = new RoutingInputWidget( d->m_routeSkeleton, index, this );
        input->setProgressAnimation( d->m_progressAnimation );
        input->setWorkOffline( d->m_workOffline );
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
            d->m_routeSkeleton->remove( index );
        }
        d->m_routingManager->updateRoute();
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
            d->m_routingLayer->setModel( d->m_routingManager->routingModel() );
        }
        d->adjustInputWidgets();
    }

    if ( d->m_inputWidgets.size() < 2 ) {
        addInputWidget();
    }
}

void RoutingWidget::updateRouteState( RoutingManager::State state, RouteSkeleton *route )
{
    Q_UNUSED( route );

    if ( state == RoutingManager::Retrieved ) {
        // Parts of the route may lie outside the route trip points
        GeoDataLineString bbox;
        for ( int i = 0; i < d->m_routingManager->routingModel()->rowCount(); ++i ) {
            QModelIndex index = d->m_routingManager->routingModel()->index( i, 0 );
            QVariant pos = index.data( RoutingModel::CoordinateRole );
            if ( !pos.isNull() ) {
                bbox << qVariantValue<GeoDataCoordinates>( pos );
            }
        }

        if ( bbox.size() > 1 ) {
            qreal distance = d->m_routingManager->routingModel()->totalDistance();
            unsigned int days = d->m_routingManager->routingModel()->duration().days;
            QTime time = d->m_routingManager->routingModel()->duration().time;
            QString timeString = time.toString( Qt::DefaultLocaleShortDate );
            if ( days ) {
                QString label = tr( "Estimated travel time: %1 days, %2 (%3 km)", 0, days );
                d->m_ui.descriptionLabel->setText( label.arg( days ).arg( timeString ).arg( distance, 0, 'f', 1 ) );
            } else {
                QString label = tr( "Estimated travel time: %1 (%2 km)" );
                d->m_ui.descriptionLabel->setText( label.arg( timeString ).arg( distance, 0, 'f', 1 ) );
            }
            d->m_ui.descriptionLabel->setVisible( true );

            if ( d->m_zoomRouteAfterDownload ) {
                d->m_zoomRouteAfterDownload = false;
                d->m_widget->centerOn( GeoDataLatLonBox::fromLineString( bbox ) );
            }
        }
    }

    d->m_routingLayer->setRouteDirty( state == RoutingManager::Downloading );

    if ( state == RoutingManager::Downloading ) {
        d->m_progressTimer.start();
    } else {
        d->m_progressTimer.stop();
        d->m_ui.searchButton->setIcon( QIcon() );
    }
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

void RoutingWidget::toggleOptionsVisibility()
{
    bool visible = !d->m_ui.routePreferenceComboBox->isVisible();
    d->m_ui.routePreferenceComboBox->setVisible( visible );
    d->m_ui.highwaysCheckBox->setVisible( visible );
    d->m_ui.tollWaysCheckBox->setVisible( visible );
    d->m_ui.preferenceLabel->setVisible( visible );
    d->m_ui.avoidLabel->setVisible( visible );
}

void RoutingWidget::exportRoute()
{
    QString fileName = QFileDialog::getSaveFileName( this, tr( "Export Route" ), // krazy:exclude=qclasses
                       QDir::homePath(),
                       tr( "GPX files (*.gpx)" ) );

    if ( !fileName.isEmpty() ) {
        QFile gpx( fileName );
        if ( gpx.open( QFile::WriteOnly) ) {
            d->m_routingManager->routingModel()->exportGpx( &gpx );
            gpx.close();
        }
    }
}

void RoutingWidget::setWorkOffline( bool offline )
{
    foreach ( RoutingInputWidget *widget, d->m_inputWidgets ) {
        widget->setWorkOffline( offline );
    }

    d->m_workOffline = offline;
    d->m_routingManager->setWorkOffline( offline );
}

void RoutingWidget::updateProgress()
{
    if ( !d->m_progressAnimation.isEmpty() ) {
        d->m_currentFrame = ( d->m_currentFrame + 1 ) % d->m_progressAnimation.size();
        QIcon frame = d->m_progressAnimation[d->m_currentFrame];
        d->m_ui.searchButton->setIcon( frame );
    }
}

} // namespace Marble

#include "RoutingWidget.moc"
