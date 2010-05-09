/* This file is part of the KDE project
 *
 * Copyright 2004-2007 Torsten Rahn  <tackat@kde.org>
 * Copyright 2007      Inge Wallin   <ingwa@kde.org>
 * Copyright 2007      Thomas Zander <zander@kde.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */


#include "MarbleControlBox.h"

#include <QtCore/QTime>
#include <QtCore/QTimer>
#include <QtGui/QSortFilterProxyModel>
#include <QtGui/QStandardItemModel>

#include "global.h"

#include "MarbleWidget.h"
#include "MarbleMap.h"
#include "MarbleModel.h"
#include "FileViewModel.h"
#include "gps/GpxFileModel.h"
#include "gps/GpsLayer.h"
#include "gps/PositionTracking.h"
#include "MarbleLocale.h"
#include "MarblePlacemarkModel.h"
#include "RoutingWidget.h"
#include "MarbleRunnerManager.h"
#include "MathHelper.h"
#include "MapThemeSortFilterProxyModel.h"
#include "GeoSceneDocument.h"
#include "GeoSceneHead.h"
#include "Planet.h"
#include "HttpDownloadManager.h"
#include "PositionProviderPlugin.h"
#include "PluginManager.h"

#include "GeoDataCoordinates.h"

using namespace Marble;

#include "ui_MarbleControlBox.h"

namespace Marble
{

class MarbleControlBoxPrivate
{
 public:
    MarbleControlBoxPrivate();
    MarbleWidget  *m_widget;
    QString        m_searchTerm;
    bool           m_searchTriggered;
    QStandardItemModel *m_celestialList;

    Ui::MarbleControlBox  uiWidget;
    QWidget              *m_navigationWidget;
    QWidget              *m_legendWidget;
    QWidget              *m_mapViewWidget;
    QWidget              *m_currentLocation2Widget;
    QWidget              *m_fileViewWidget;

    QStandardItemModel     *m_mapThemeModel;
    QSortFilterProxyModel  *m_sortproxy;
    MapThemeSortFilterProxyModel *m_mapSortProxy;

    MarbleRunnerManager  *m_runnerManager;
    RoutingWidget  *m_routingWidget;
    GeoSceneDocument      *mapTheme;

    MarbleLocale* m_locale;
    QList<PositionProviderPlugin*> m_positionProviderPlugins;
    GeoDataCoordinates m_currentPosition;
};

MarbleControlBoxPrivate::MarbleControlBoxPrivate() : m_routingWidget(0)
{
}

// ================================================================


MarbleControlBox::MarbleControlBox(QWidget *parent)
    : QWidget( parent ),
      d( new MarbleControlBoxPrivate )
{
    d->m_widget = 0;
    d->m_searchTerm.clear();
    d->m_searchTriggered = false;
    d->uiWidget.setupUi( this );

    setFocusPolicy( Qt::NoFocus );
//    setFocusProxy( d->uiWidget.searchLineEdit );

    //  Iterate through all of the Side Widget values  //
    d->uiWidget.toolBox->setCurrentIndex( 0 );
    d->m_navigationWidget = d->uiWidget.toolBox->currentWidget();

    d->uiWidget.toolBox->setCurrentIndex( 1 );
    d->m_legendWidget = d->uiWidget.toolBox->currentWidget();

    d->uiWidget.toolBox->setCurrentIndex( 2 );
    d->m_mapViewWidget = d->uiWidget.toolBox->currentWidget();

    d->uiWidget.toolBox->setCurrentIndex( 3 );
    d->m_fileViewWidget = d->uiWidget.toolBox->currentWidget();

    d->uiWidget.toolBox->setCurrentIndex( 4 );
    d->m_currentLocation2Widget = d->uiWidget.toolBox->currentWidget();

    d->m_sortproxy = new QSortFilterProxyModel( d->uiWidget.locationListView );
    d->uiWidget.locationListView->setModel( d->m_sortproxy );

    d->m_mapSortProxy = new MapThemeSortFilterProxyModel( this );

    d->uiWidget.toolBox->setCurrentIndex(0);

    d->m_locale = MarbleGlobal::getInstance()->locale();

    //default
    setCurrentLocationTabShown( true );
    setFileViewTabShown( false );

    connect( d->uiWidget.goHomeButton,  SIGNAL( clicked() ),
             this,                      SIGNAL( goHome() ) );
    connect( d->uiWidget.zoomSlider,    SIGNAL( valueChanged( int ) ),
             this,                      SIGNAL( zoomChanged( int ) ) );
    connect( d->uiWidget.zoomInButton,  SIGNAL( clicked() ),
             this,                      SIGNAL( zoomIn() ) );
    connect( d->uiWidget.zoomOutButton, SIGNAL( clicked() ),
             this,                      SIGNAL( zoomOut() ) );

    connect( d->uiWidget.zoomSlider,  SIGNAL( valueChanged( int ) ),
             this,                      SLOT( updateButtons( int ) ) );

    connect( d->uiWidget.moveLeftButton,  SIGNAL( clicked() ),
             this,                        SIGNAL( moveLeft() ) );
    connect( d->uiWidget.moveRightButton, SIGNAL( clicked() ),
             this,                        SIGNAL( moveRight() ) );
    connect( d->uiWidget.moveUpButton,    SIGNAL( clicked() ),
             this,                        SIGNAL( moveUp() ) );
    connect( d->uiWidget.moveDownButton,  SIGNAL( clicked() ),
             this,                        SIGNAL (moveDown() ) );

    connect( d->uiWidget.locationListView, SIGNAL( centerOn( const QModelIndex& ) ),
             this,                         SLOT( mapCenterOnSignal( const QModelIndex& ) ) );

    d->m_mapThemeModel = 0;

    connect( d->uiWidget.marbleThemeSelectView, SIGNAL( selectMapTheme( const QString& ) ),
             this,                              SIGNAL( selectMapTheme( const QString& ) ) );
    connect( d->uiWidget.projectionComboBox,    SIGNAL( activated( int ) ),
             this,                              SLOT( projectionSelected( int ) ) );
    connect( d->uiWidget.zoomSlider,  SIGNAL( sliderPressed() ),
             this,                      SLOT( adjustForAnimation() ) );
    connect( d->uiWidget.zoomSlider,  SIGNAL( sliderReleased() ),
             this,                      SLOT( adjustForStill() ) );

    d->uiWidget.projectionComboBox->setEnabled( true );

    d->m_runnerManager = new MarbleRunnerManager( this );

    connect( d->m_runnerManager, SIGNAL( modelChanged(  MarblePlacemarkModel* ) ),
             this,               SLOT( setLocations( MarblePlacemarkModel* ) ) );

    connect( d->uiWidget.searchLineEdit,  SIGNAL( textChanged( const QString& ) ),
             this,                        SLOT( searchLineChanged( const QString& ) ) );
    connect( d->uiWidget.searchLineEdit,  SIGNAL( returnPressed() ),
             this,                        SLOT( searchReturnPressed() ) );

    // Setting up the celestial combobox
    d->m_celestialList = new QStandardItemModel();
    d->uiWidget.celestialBodyComboBox->setModel( d->m_celestialList );
    connect( d->uiWidget.celestialBodyComboBox, SIGNAL( activated( const QString& ) ),
             this,                              SLOT( selectCurrentMapTheme( const QString& ) ) );
}

MarbleControlBox::~MarbleControlBox()
{
    delete d->m_celestialList;
    delete d;
}

void MarbleControlBox::setMapThemeModel( QStandardItemModel *mapThemeModel )
{
    if ( mapThemeModel != d->m_mapThemeModel ) {
        delete d->m_mapThemeModel;
    }

    d->m_mapThemeModel = mapThemeModel;
    d->m_mapSortProxy->setSourceModel( d->m_mapThemeModel );
    int currentIndex = d->uiWidget.celestialBodyComboBox->currentIndex();
    QStandardItem * selectedItem = d->m_celestialList->item( currentIndex, 1 );

    if ( selectedItem ) {
        QString selectedId;
        selectedId = selectedItem->text();
        d->m_mapSortProxy->setFilterRegExp( QRegExp( selectedId, Qt::CaseInsensitive,QRegExp::FixedString ) );
    }

    d->m_mapSortProxy->sort( 0 );
    d->uiWidget.marbleThemeSelectView->setModel( d->m_mapSortProxy );
    connect( d->m_mapThemeModel,       SIGNAL( rowsInserted( QModelIndex, int, int ) ),
            this,                     SLOT( updateMapThemeView() ) );
}

void MarbleControlBox::updateCelestialModel()
{
    int row = d->m_mapThemeModel->rowCount();

    for ( int i = 0; i < row; ++i )
    {
        QString celestialBodyId = ( d->m_mapThemeModel->data( d->m_mapThemeModel->index( i, 1 ) ).toString() ).section( '/', 0, 0 );
        QString celestialBodyName = Planet::name( celestialBodyId );

        QList<QStandardItem*> matchingItems = d->m_celestialList->findItems ( celestialBodyId, Qt::MatchExactly, 1 );
        if ( matchingItems.isEmpty() ) {
            d->m_celestialList->appendRow( QList<QStandardItem*>()
                                << new QStandardItem( celestialBodyName )
                                << new QStandardItem( celestialBodyId ) );
        }
    }
}

void MarbleControlBox::updateButtons( int value )
{
    if ( value <= d->uiWidget.zoomSlider->minimum() ) {
        d->uiWidget.zoomInButton->setEnabled( true );
        d->uiWidget.zoomOutButton->setEnabled( false );
    } else if ( value >= d->uiWidget.zoomSlider->maximum() ) {
        d->uiWidget.zoomInButton->setEnabled( false );
        d->uiWidget.zoomOutButton->setEnabled( true );
    } else {
        d->uiWidget.zoomInButton->setEnabled( true );
        d->uiWidget.zoomOutButton->setEnabled( true );
    }
}


void MarbleControlBox::addMarbleWidget(MarbleWidget *widget)
{
    d->m_widget = widget;
    d->m_runnerManager->setMap( d->m_widget->map() );

    d->m_routingWidget = new RoutingWidget( widget, this );
    d->uiWidget.toolBox->addItem( d->m_routingWidget, tr( "Routing" ) );

    // Make us aware of all the Placemarks in the MarbleModel so that
    // we can search them.
    setLocations( static_cast<MarblePlacemarkModel*>(d->m_widget->placemarkModel()) );

//    FIXME: Why does this fail: "selection model works on a different model than the view..." ?
//    d->uiWidget.locationListView->setSelectionModel( d->m_widget->placemarkSelectionModel() );

    //set up everything for the FileModel
    d->uiWidget.m_fileView->setModel( widget->fileViewModel() );
    delete d->uiWidget.m_fileView->selectionModel();
    d->uiWidget.m_fileView->setSelectionModel(
            widget->fileViewModel()->selectionModel());
    connect( d->uiWidget.m_fileView->selectionModel(),
	     SIGNAL( selectionChanged( QItemSelection, QItemSelection )),
	     this,
	     SLOT( enableFileViewActions() ) );
    connect( d->uiWidget.m_saveButton,  SIGNAL( clicked() ) ,
             widget->fileViewModel(),    SLOT( saveFile() ) );
    connect( d->uiWidget.m_closeButton, SIGNAL( clicked() ) ,
             widget->fileViewModel(),    SLOT( closeFile() ) );

    // Initialize the MarbleLegendBrowser
    d->uiWidget.marbleLegendBrowser->setMarbleWidget( d->m_widget );

    // Connect necessary signals.
    connect( this, SIGNAL(goHome()),         d->m_widget, SLOT(goHome()) );
    connect( this, SIGNAL(zoomChanged(int)), d->m_widget, SLOT(zoomView(int)) );
    connect( this, SIGNAL(zoomIn()),         d->m_widget, SLOT(zoomIn()) );
    connect( this, SIGNAL(zoomOut()),        d->m_widget, SLOT(zoomOut()) );

    connect( this, SIGNAL(moveLeft()),  d->m_widget, SLOT(moveLeft()) );
    connect( this, SIGNAL(moveRight()), d->m_widget, SLOT(moveRight()) );
    connect( this, SIGNAL(moveUp()),    d->m_widget, SLOT(moveUp()) );
    connect( this, SIGNAL(moveDown()),  d->m_widget, SLOT(moveDown()) );

    connect( this,        SIGNAL( projectionSelected( Projection ) ),
             d->m_widget, SLOT( setProjection( Projection ) ) );

    connect( d->m_widget, SIGNAL( themeChanged( QString ) ),
             this,        SLOT( selectTheme( QString ) ) );


    connect( d->m_widget, SIGNAL( projectionChanged( Projection ) ),
             this,        SLOT( selectProjection( Projection ) ) );
    selectProjection( d->m_widget->projection() );

    connect( d->m_widget, SIGNAL( zoomChanged( int ) ),
             this,        SLOT( changeZoom( int ) ) );
    connect( this,        SIGNAL( centerOn( const QModelIndex&, bool ) ),
             d->m_widget, SLOT( centerOn( const QModelIndex&, bool ) ) );
    connect( this,        SIGNAL( selectMapTheme( const QString& ) ),
             d->m_widget, SLOT( setMapThemeId( const QString& ) ) );

    // connect signals for the Legend

    connect( d->uiWidget.marbleLegendBrowser, SIGNAL( toggledShowProperty( QString, bool ) ),
             d->m_widget,                     SLOT( setPropertyValue( QString, bool ) ) );

    PluginManager* pluginManager = d->m_widget->model()->pluginManager();
    d->m_positionProviderPlugins = pluginManager->createPositionProviderPlugins();
    foreach( const PositionProviderPlugin *plugin, d->m_positionProviderPlugins ) {
       d->uiWidget.positionTrackingComboBox->addItem( plugin->guiString() );
    }

    //connect CurrentLoctaion signals
    connect( this, SIGNAL( gpsInputDisabled( bool ) ),
             d->m_widget, SLOT( setShowGps( bool ) ) );
    connect( this, SIGNAL( gpsPositionChanged( qreal, qreal ) ),
             d->m_widget, SLOT( changeCurrentPosition( qreal, qreal ) ) );
    connect( d->m_widget->model()->gpsLayer()->getPositionTracking(),
             SIGNAL( gpsLocation( GeoDataCoordinates, qreal ) ),
             this, SLOT( receiveGpsCoordinates( GeoDataCoordinates, qreal ) ) );
    connect( d->uiWidget.positionTrackingComboBox, SIGNAL( currentIndexChanged( QString ) ),
             this, SLOT( changePositionProvider( QString ) ) );
    connect( d->uiWidget.locationLabel, SIGNAL( linkActivated( QString ) ),
             this, SLOT( centerOnCurrentLocation() ) );
    connect( d->m_widget->model()->gpsLayer()->getPositionTracking(),
             SIGNAL( statusChanged( PositionProviderStatus) ), this,
             SLOT( adjustPositionTrackingStatus( PositionProviderStatus) ) );
}

void MarbleControlBox::setWidgetTabShown( QWidget * widget,
                                          int insertIndex, bool show,
                                          QString &text )
{
    int index = d->uiWidget.toolBox->indexOf( widget );

    if( show ) {
        if ( !(index >= 0) ){
            if ( insertIndex < d->uiWidget.toolBox->count() ) {
                d->uiWidget.toolBox->insertItem( insertIndex,
                                                 widget,
                                                 text );
            } else {
                d->uiWidget.toolBox->insertItem( 3 ,widget, text );
            }
            widget->show();
        }
    } else {
        if ( index >= 0 ) {
            widget->hide();
            d->uiWidget.toolBox->removeItem( index );
        }
    }
}


void MarbleControlBox::setLocations(MarblePlacemarkModel* locations)
{
    QTime t;
    t.start();
    d->m_sortproxy->setSourceModel( locations );
    d->m_sortproxy->setSortLocaleAware( true );
    d->m_sortproxy->setDynamicSortFilter( false );
    d->m_sortproxy->sort( 0 );
    qDebug("MarbleControlBox (sort): Time elapsed: %d ms", t.elapsed());
}

int MarbleControlBox::minimumZoom() const
{
    return d->m_widget->minimumZoom();
}

void MarbleControlBox::updateMapThemeView()
{
    updateCelestialModel();

    if ( d->m_widget ) {
        QString mapThemeId = d->m_widget->mapThemeId();
        if ( !mapThemeId.isEmpty() )
            selectTheme( mapThemeId );
    }
}

void MarbleControlBox::changeZoom(int zoom)
{
    // There exists a circular signal/slot connection between MarbleWidget and this widget's
    // zoom slider. MarbleWidget prevents recursion, but it still loops one time unless
    // disconnected here. Note that it would be possible to only connect the sliders
    // sliderMoved signal instead of its valueChanged signal above to break up the loop.
    // This however means that the slider cannot be operated with the mouse wheel, as this
    // does not emit the sliderMoved signal for some reason. Therefore the signal is disconnected
    // below before calling setValue on the slider to avoid that it calls back to MarbleWidget,
    // and then connected again to make user interaction possible.

    disconnect( d->uiWidget.zoomSlider,    SIGNAL( valueChanged( int ) ),
             this,                      SIGNAL( zoomChanged( int ) ) );

    d->uiWidget.zoomSlider->setValue( zoom );
    d->uiWidget.zoomSlider->setMinimum( minimumZoom() );

    connect( d->uiWidget.zoomSlider,    SIGNAL( valueChanged( int ) ),
             this,                      SIGNAL( zoomChanged( int ) ) );
}

void MarbleControlBox::adjustPositionTrackingStatus( PositionProviderStatus status )
{
    if ( status == PositionProviderStatusAvailable ) {
        return;
    }

    QString html = "<html><body><p>";

    switch ( status ) {
        case PositionProviderStatusUnavailable:
            html += tr( "Waiting for current location information..." );
            break;
        case PositionProviderStatusAcquiring:
            html += tr( "Initializing current location service..." );
            break;
        case PositionProviderStatusAvailable:
            Q_ASSERT( false );
            break;
        case PositionProviderStatusError:
            html += tr( "Error when determining current location: " );
            html += d->m_widget->model()->gpsLayer()->getPositionTracking()->error();
            break;
    }

    html += "</p></body></html>";
    d->uiWidget.locationLabel->setEnabled( true );
    d->uiWidget.locationLabel->setText( html );
}

void MarbleControlBox::receiveGpsCoordinates( const GeoDataCoordinates &position, qreal speed )
{
    d->m_currentPosition = position;
    QString unitString;
    QString speedString;
    QString distanceUnitString;
    QString distanceString;
    qreal unitSpeed = 0.0;
    qreal distance = 0.0;

    QString html = "<html><body>";
    html += "<table cellspacing=\"2\" cellpadding=\"2\">";
    html += "<tr><td>Longitude</td><td><a href=\"http://edu.kde.org/marble\">%1</a></td></tr>";
    html += "<tr><td>Latitude</td><td><a href=\"http://edu.kde.org/marble\">%2</a></td></tr>";
    html += "<tr><td>Altitude</td><td>%3</td></tr>";
    html += "<tr><td>Speed</td><td>%4</td></tr>";
    html += "</table>";
    html += "</body></html>";

    switch ( d->m_locale->measureSystem() ) {
        case Metric:
        //kilometers per hour
        unitString = tr("km/h");
        unitSpeed = speed / 1000;
        distanceUnitString = tr("m");
        distance = position.altitude();
        break;

        case Imperial:
        //miles per hour
        unitString = tr("m/h");
        unitSpeed = (speed/1000) * KM2MI;
        distanceUnitString = tr("ft");
        distance = position.altitude() * M2FT;
        break;
    }
    // TODO read this value from the incoming signal
    speedString = QLocale::system().toString( unitSpeed, 'f', 1);
    distanceString = QString( "%1 %2" ).arg( distance, 0, 'f', 1, QChar(' ') ).arg( distanceUnitString );

    html = html.arg( position.lonToString() ).arg( position.latToString() );
    html = html.arg( distanceString ).arg( speedString + ' ' + unitString );
    d->uiWidget.locationLabel->setText( html );
}


void MarbleControlBox::enableFileViewActions()
{
    bool tmp = d->uiWidget.m_fileView->selectionModel()
            ->selectedIndexes().count() ==1;
    d->uiWidget.m_saveButton->setEnabled( tmp );
    d->uiWidget.m_closeButton->setEnabled( tmp );

#ifndef KML_GSOC
    if ( tmp ) {
        QModelIndex tmpIndex =
            d->uiWidget.m_fileView->selectionModel()->currentIndex();
        d->m_widget->gpxFileModel()->setSelectedIndex( tmpIndex );
    }
#else
    if ( tmp ) {
        FileViewModel& model = ( FileViewModel& ) *d->uiWidget.m_fileView->model();
        model.setSelectedIndex( d->uiWidget.m_fileView->selectionModel()->currentIndex() );
    }
#endif
}

void MarbleControlBox::setNavigationTabShown( bool show )
{
    QString  title = tr( "Navigation" );
    setWidgetTabShown( d->m_navigationWidget, 0, show, title);
}

void MarbleControlBox::setLegendTabShown( bool show )
{
    QString  title = tr( "Legend" );
    setWidgetTabShown( d->m_legendWidget, 1, show, title );
}

void MarbleControlBox::setMapViewTabShown( bool show )
{
    QString  title = tr( "Map View" );
    setWidgetTabShown( d->m_mapViewWidget, 2, show, title );
}

void MarbleControlBox::setFileViewTabShown( bool show )
{
    QString  title = tr( "File View" );
    setWidgetTabShown( d->m_fileViewWidget, 3, show, title );
}

void MarbleControlBox::setCurrentLocationTabShown( bool show )
{
    QString  title = tr( "Current Location" );
    setWidgetTabShown( d->m_currentLocation2Widget, 4, show, title );
    if ( d->m_widget ) {
        bool enabled = d->m_widget->mapTheme()->head()->target() == "earth";
        int locationIndex = d->uiWidget.toolBox->indexOf( d->m_currentLocation2Widget );
        if ( locationIndex >= 0 ) {
            d->uiWidget.toolBox->setItemEnabled( locationIndex, enabled );
        }

        if ( !enabled ) {
            d->m_widget->map()->model()->gpsLayer()->setVisible( false );
        }
    }
}



void MarbleControlBox::resizeEvent ( QResizeEvent * )
{
    if ( height() < 500 ) {
        if ( !d->uiWidget.zoomSlider->isHidden() ) {
            setUpdatesEnabled(false);
            d->uiWidget.zoomSlider->hide();
            d->uiWidget.m_pSpacerFrame->setSizePolicy( QSizePolicy::Preferred,
                                                       QSizePolicy::Expanding );
            setUpdatesEnabled(true);
        }
    } else {
        if ( d->uiWidget.zoomSlider->isHidden() ) {
            setUpdatesEnabled(false);
            d->uiWidget.zoomSlider->show();
            d->uiWidget.m_pSpacerFrame->setSizePolicy( QSizePolicy::Preferred,
                                                       QSizePolicy::Fixed );
            setUpdatesEnabled(true);
        }
    }
}

void MarbleControlBox::searchLineChanged(const QString &search)
{
    d->m_searchTerm = search;
    // if search line is empty, restore original geonames
    if ( d->m_searchTerm.isEmpty() )
        setLocations( static_cast<MarblePlacemarkModel*>( d->m_widget->placemarkModel() ) );
    if ( d->m_searchTriggered )
        return;
    d->m_searchTriggered = true;
    QTimer::singleShot( 0, this, SLOT( search() ) );
}

void MarbleControlBox::searchReturnPressed()
{
    // do nothing if search term empty
    if ( !d->m_searchTerm.isEmpty() ) {
        d->m_runnerManager->newText( d->m_searchTerm );
    }
}


void MarbleControlBox::search()
{
    d->m_searchTriggered = false;
    int  currentSelected = d->uiWidget.locationListView->currentIndex().row();
    d->uiWidget.locationListView->selectItem( d->m_searchTerm );
    if ( currentSelected != d->uiWidget.locationListView->currentIndex().row() )
        d->uiWidget.locationListView->activate();
}

void MarbleControlBox::selectTheme( const QString &theme )
{
    if ( !d->m_mapSortProxy || !d->m_widget )
        return;
    // Check if the new selected theme is different from the current one
    QModelIndex currentIndex = d->uiWidget.marbleThemeSelectView->currentIndex();
    QString indexTheme = d->m_mapSortProxy->data( d->m_mapSortProxy->index(
                         currentIndex.row(), 1, QModelIndex() ) ).toString();


    d->uiWidget.zoomSlider->setMaximum( d->m_widget->map()->maximumZoom() );
    updateButtons( d->uiWidget.zoomSlider->value() );

    if ( theme != indexTheme ) {
        /* indexTheme would be empty if the chosen map has not been set yet. As
        this needs to be done after the mapThemeId has been set, check if that is
        not empty first. The behaviour differs between Linux and Windows: on
        Windows the reading of the settings is not delayed, thus the mapThemeId
        is available earlier than on Linux.
        */
        if( indexTheme.isEmpty() && !d->m_widget->mapThemeId().isEmpty() ) {
            QList<QStandardItem*> items = d->m_mapThemeModel->findItems( theme, Qt::MatchExactly, 1 );
            if( items.size() >= 1 ) {
                QModelIndex iterIndex = items.first()->index();
                QModelIndex iterIndexName = d->m_mapSortProxy->mapFromSource( iterIndex.sibling( iterIndex.row(), 0 ) );

                d->uiWidget.marbleThemeSelectView->setCurrentIndex( iterIndexName );

                d->uiWidget.marbleThemeSelectView->scrollTo( iterIndexName );
            }
        }

        QString selectedId = d->m_widget->mapTheme()->head()->target();
        d->m_runnerManager->setCelestialBodyId( selectedId );
        int routingIndex = d->uiWidget.toolBox->indexOf( d->m_routingWidget );
        d->uiWidget.toolBox->setItemEnabled( routingIndex, selectedId == "earth" );
        int locationIndex = d->uiWidget.toolBox->indexOf( d->m_currentLocation2Widget );
        if ( locationIndex >= 0 ) {
            d->uiWidget.toolBox->setItemEnabled( locationIndex, selectedId == "earth" );
        }

        QList<QStandardItem*> itemList = d->m_celestialList->findItems( selectedId, Qt::MatchExactly, 1 );

        if ( !itemList.isEmpty() ) {
            QStandardItem * selectedItem = itemList.first();

            if ( selectedItem ) {
                int selectedIndex = selectedItem->row();
                d->uiWidget.celestialBodyComboBox->setCurrentIndex( selectedIndex );
                d->m_mapSortProxy->setFilterRegExp( QRegExp( selectedId, Qt::CaseInsensitive,QRegExp::FixedString ) );
            }

            d->m_mapSortProxy->sort( 0 );
        }
    }
}

void MarbleControlBox::selectProjection( Projection projection )
{
    if ( (int)projection != d->uiWidget.projectionComboBox->currentIndex() )
        d->uiWidget.projectionComboBox->setCurrentIndex( (int) projection );
}

void MarbleControlBox::selectCurrentMapTheme( const QString& celestialBodyId )
{
    Q_UNUSED( celestialBodyId )

    setMapThemeModel( d->m_mapThemeModel );

    bool foundMapTheme = false;

    QString currentMapThemeId = d->m_widget->mapThemeId();

    int row = d->m_mapSortProxy->rowCount();

    for ( int i = 0; i < row; ++i )
    {
        QModelIndex index = d->m_mapSortProxy->index(i,1);
        QString itMapThemeId = d->m_mapSortProxy->data(index).toString();
        if ( currentMapThemeId == itMapThemeId )
        {
            foundMapTheme = true;
            break;
        }
    }
    if ( !foundMapTheme ) {
        QModelIndex index = d->m_mapSortProxy->index(0,1);
        d->m_widget->setMapThemeId( d->m_mapSortProxy->data(index).toString());
    }

    updateMapThemeView();
}
// Relay a signal and convert the parameter from an int to a Projection.
void MarbleControlBox::projectionSelected( int projectionIndex )
{
    emit projectionSelected( (Projection) projectionIndex );
}

void MarbleControlBox::mapCenterOnSignal( const QModelIndex &index )
{
    emit centerOn( d->m_sortproxy->mapToSource( index ), true );
}

void MarbleControlBox::adjustForAnimation()
{
    // TODO: use signals here as well
    if ( !d->m_widget )
        return;

    d->m_widget->setViewContext( Animation );
}

void MarbleControlBox::adjustForStill()
{
    // TODO: use signals here as well
    if ( !d->m_widget )
        return;

    d->m_widget->setViewContext( Still );

    if ( d->m_widget->mapQuality( Still )
        != d->m_widget->mapQuality( Animation ) )
    {
        d->m_widget->updateChangedMap();
    }
}

void MarbleControlBox::setWorkOffline(bool offline)
{
    HttpDownloadManager * const downloadManager =
        d->m_widget->map()->model()->downloadManager();
    downloadManager->setDownloadEnabled( !offline );
    d->m_runnerManager->setWorkOffline( offline );
    if ( d->m_routingWidget ) {
        d->m_routingWidget->setWorkOffline( offline );
    }
}

void MarbleControlBox::changePositionProvider( const QString &provider )
{
    if ( provider == tr("Disabled") ) {
        d->uiWidget.locationLabel->setEnabled( false );
        d->m_widget->map()->model()->gpsLayer()->setVisible( false );
        d->m_widget->model()->gpsLayer()->getPositionTracking()->setPositionProviderPlugin( 0 );
        d->m_widget->update();
    }
    else {
        foreach( PositionProviderPlugin* plugin, d->m_positionProviderPlugins ) {
            if ( plugin->guiString() == provider ) {
               d->uiWidget.locationLabel->setEnabled( true );
               PositionProviderPlugin* instance = plugin->newInstance();
               PositionTracking *tracking = d->m_widget->model()->gpsLayer()->getPositionTracking();
               tracking->setPositionProviderPlugin( instance );
               d->m_widget->map()->model()->gpsLayer()->setVisible( true );
               d->m_widget->update();
               return;
            }
        }
    }
}

void MarbleControlBox::centerOnCurrentLocation()
{
    d->m_widget->centerOn(d->m_currentPosition, true);
}

}

#include "MarbleControlBox.moc"
