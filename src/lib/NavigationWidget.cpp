///
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2004-2007 Torsten Rahn  <tackat@kde.org>
// Copyright 2007      Inge Wallin   <ingwa@kde.org>
// Copyright 2007      Thomas Zander <zander@kde.org>
// Copyright 2010      Bastian Holst <bastianholst@gmx.de>
//

// Self
#include "NavigationWidget.h"

// Marble
#include "MarbleDebug.h"
#include "MarbleModel.h"
#include "MarbleWidget.h"
#include "MarbleRunnerManager.h"
#include "MarblePlacemarkModel.h"
#include "BranchFilterProxyModel.h"
#include "GeoDataDocument.h"
#include "GeoDataTreeModel.h"
#include "GeoSceneDocument.h"
#include "GeoSceneHead.h"
#include "ViewportParams.h"

// Qt
#include <QtCore/QTime>
#include <QtCore/QTimer>
#include <QtGui/QSortFilterProxyModel>

using namespace Marble;

#include "ui_NavigationWidget.h"

namespace Marble
{

class NavigationWidgetPrivate
{
 public:
    NavigationWidgetPrivate();

    void updateButtons( int );

    void mapCenterOnSignal( const QModelIndex & );

    void adjustForAnimation();
    void adjustForStill();

    /**
     * @brief Set a list/set of placemark names for the search widget.
     * @param locations  the QAbstractitemModel containing the placemarks
     *
     * This function is called to display a potentially large number
     * of placemark names in a widget and let the user search between
     * them.
     * @see centerOn
     */
    void setSearchResult( QVector<GeoDataPlacemark*> locations );

    Ui::NavigationWidget    m_navigationUi;
    MarbleWidget           *m_widget;
    BranchFilterProxyModel  m_branchfilter;
    QSortFilterProxyModel  *m_sortproxy;
    QString                 m_searchTerm;
    MarbleRunnerManager    *m_runnerManager;
    GeoDataDocument        *m_document;
};

NavigationWidgetPrivate::NavigationWidgetPrivate()
    : m_widget( 0 ), m_sortproxy( 0 ), m_runnerManager( 0 ),
      m_document( new GeoDataDocument ) {
    m_document->setDocumentRole( SearchResultDocument );
    m_document->setName( "Search Results" );
}


NavigationWidget::NavigationWidget( QWidget *parent, Qt::WindowFlags f )
    : QWidget( parent, f ),
      d( new NavigationWidgetPrivate() )
{
    d->m_searchTerm.clear();
    d->m_widget = 0;

    d->m_navigationUi.setupUi( this );
    d->m_navigationUi.m_splitter->setStretchFactor( 0, 1 );
    d->m_navigationUi.m_splitter->setStretchFactor( 1, 2 );
    d->m_navigationUi.locationListView->setVisible( false );

    d->m_sortproxy = new QSortFilterProxyModel( this );
    d->m_navigationUi.locationListView->setModel( d->m_sortproxy );
    connect( d->m_navigationUi.goHomeButton,     SIGNAL( clicked() ),
             this,                               SIGNAL( goHome() ) );
    connect( d->m_navigationUi.zoomSlider,       SIGNAL( valueChanged( int ) ),
             this,                               SIGNAL( zoomChanged( int ) ) );
    connect( d->m_navigationUi.zoomInButton,     SIGNAL( clicked() ),
             this,                               SIGNAL( zoomIn() ) );
    connect( d->m_navigationUi.zoomOutButton,    SIGNAL( clicked() ),
             this,                               SIGNAL( zoomOut() ) );

    connect( d->m_navigationUi.zoomSlider,       SIGNAL( valueChanged( int ) ),
             this,                               SLOT( updateButtons( int ) ) );

    connect( d->m_navigationUi.moveLeftButton,   SIGNAL( clicked() ),
             this,                               SIGNAL( moveLeft() ) );
    connect( d->m_navigationUi.moveRightButton,  SIGNAL( clicked() ),
             this,                               SIGNAL( moveRight() ) );
    connect( d->m_navigationUi.moveUpButton,     SIGNAL( clicked() ),
             this,                               SIGNAL( moveUp() ) );
    connect( d->m_navigationUi.moveDownButton,   SIGNAL( clicked() ),
             this,                               SIGNAL( moveDown() ) );

    connect( d->m_navigationUi.locationListView, SIGNAL( activated( const QModelIndex& ) ),
             this,                               SLOT( mapCenterOnSignal( const QModelIndex& ) ) );

    connect( d->m_navigationUi.zoomSlider,       SIGNAL( sliderPressed() ),
             this,                               SLOT( adjustForAnimation() ) );
    connect( d->m_navigationUi.zoomSlider,       SIGNAL( sliderReleased() ),
             this,                               SLOT( adjustForStill() ) );
}

NavigationWidget::~NavigationWidget()
{
    delete d;
}

void NavigationWidget::setMarbleWidget( MarbleWidget *widget )
{
    GeoDataTreeModel *treeModel;

    d->m_runnerManager = new MarbleRunnerManager( widget->model()->pluginManager(), this );
    connect( d->m_runnerManager, SIGNAL( searchResultChanged( QVector<GeoDataPlacemark*> ) ),
             this,               SLOT( setSearchResult( QVector<GeoDataPlacemark*> ) ) );
    connect( d->m_runnerManager, SIGNAL( searchFinished( QString ) ), this, SIGNAL( searchFinished() ) );

    d->m_widget = widget;
    d->m_runnerManager->setModel( widget->model() );
    treeModel = d->m_widget->model()->treeModel();
    treeModel->addDocument( d->m_document );

    d->m_branchfilter.setSourceModel( treeModel );
    d->m_branchfilter.setBranchIndex( treeModel, treeModel->index( d->m_document ) );
    d->m_sortproxy->setSortRole( MarblePlacemarkModel::PopularityIndexRole );
    d->m_sortproxy->sort( 0, Qt::AscendingOrder );
    d->m_sortproxy->setDynamicSortFilter( true );
    d->m_sortproxy->setSourceModel( &d->m_branchfilter );
    d->m_navigationUi.locationListView->setRootIndex(
                d->m_sortproxy->mapFromSource(
                    d->m_branchfilter.mapFromSource( treeModel->index( d->m_document ) ) ) );

    // Connect necessary signals.
    connect( this, SIGNAL( goHome() ),         d->m_widget, SLOT( goHome() ) );
    connect( this, SIGNAL( zoomChanged(int) ), d->m_widget, SLOT( zoomView( int ) ) );
    connect( this, SIGNAL( zoomIn() ),         d->m_widget, SLOT( zoomIn() ) );
    connect( this, SIGNAL( zoomOut() ),        d->m_widget, SLOT( zoomOut() ) );

    connect( this, SIGNAL( moveLeft() ),  d->m_widget, SLOT( moveLeft() ) );
    connect( this, SIGNAL( moveRight() ), d->m_widget, SLOT( moveRight() ) );
    connect( this, SIGNAL( moveUp() ),    d->m_widget, SLOT( moveUp() ) );
    connect( this, SIGNAL( moveDown() ),  d->m_widget, SLOT( moveDown() ) );

    connect( d->m_widget, SIGNAL( zoomChanged( int ) ),
             this,        SLOT( changeZoom( int ) ) );

    connect( d->m_widget, SIGNAL( themeChanged( QString ) ),
             this,        SLOT( selectTheme( QString ) ) );
}

void NavigationWidget::search(const QString &searchTerm, SearchMode searchMode )
{
    d->m_searchTerm = searchTerm;

    if( searchTerm.isEmpty() ) {
        clearSearch();
    } else {
        d->m_navigationUi.locationListView->setVisible( true );
        if ( searchMode == AreaSearch ) {
            d->m_runnerManager->findPlacemarks( d->m_searchTerm, d->m_widget->viewport()->viewLatLonAltBox() );
        } else {
            d->m_runnerManager->findPlacemarks( d->m_searchTerm );
        }
    }
}

void NavigationWidget::changeZoom( int zoom )
{
    // There exists a circular signal/slot connection between MarbleWidget and this widget's
    // zoom slider. MarbleWidget prevents recursion, but it still loops one time unless
    // blocked here. Note that it would be possible to only connect the sliders
    // sliderMoved signal instead of its valueChanged signal above to break up the loop.
    // This however means that the slider cannot be operated with the mouse wheel, as this
    // does not emit the sliderMoved signal for some reason. Therefore the signal is blocked
    // below before calling setValue on the slider to avoid that it calls back to MarbleWidget,
    // and then un-blocked again to make user interaction possible.

    d->m_navigationUi.zoomSlider->blockSignals( true );

    d->m_navigationUi.zoomSlider->setValue( zoom );
    // As we have disabled all zoomSlider Signals, we have to update our buttons separately.
    d->updateButtons( zoom );

    d->m_navigationUi.zoomSlider->blockSignals( false );
}

void NavigationWidget::clearSearch()
{
    d->m_searchTerm.clear();

    d->m_navigationUi.locationListView->setVisible( false );
    d->m_widget->model()->placemarkSelectionModel()->clear();

    // clear the local document
    GeoDataTreeModel *treeModel = d->m_widget->model()->treeModel();
    treeModel->removeDocument( d->m_document );
    d->m_document->clear();
    treeModel->addDocument( d->m_document );
    d->m_branchfilter.setBranchIndex( treeModel, treeModel->index( d->m_document ) );
    d->m_navigationUi.locationListView->setRootIndex(
            d->m_sortproxy->mapFromSource(
                d->m_branchfilter.mapFromSource( treeModel->index( d->m_document ) ) ) );

    // clear cached search results
    d->m_runnerManager->findPlacemarks( QString() );
}

void NavigationWidgetPrivate::setSearchResult( QVector<GeoDataPlacemark*> locations )
{
    if( locations.isEmpty() ) {
        return;
    }

    QTime t;
    t.start();

    // fill the local document with results
    m_widget->model()->placemarkSelectionModel()->clear();
    GeoDataTreeModel *treeModel = m_widget->model()->treeModel();
    treeModel->removeDocument( m_document );
    m_document->clear();
    foreach (GeoDataPlacemark *placemark, locations ) {
        m_document->append( new GeoDataPlacemark( *placemark ) );
    }
    treeModel->addDocument( m_document );
    m_branchfilter.setBranchIndex( treeModel, treeModel->index( m_document ) );
    m_navigationUi.locationListView->setRootIndex(
                m_sortproxy->mapFromSource(
                    m_branchfilter.mapFromSource( treeModel->index( m_document ) ) ) );
    m_widget->centerOn( m_document->latLonAltBox() );
    mDebug() << "NavigationWidget (searchResults): Time elapsed:"<< t.elapsed() << " ms";
}

void NavigationWidget::selectTheme( const QString &theme )
{
    Q_UNUSED( theme )

    if( d->m_widget ) {
        d->m_navigationUi.zoomSlider->setMinimum( d->m_widget->minimumZoom() );
        d->m_navigationUi.zoomSlider->setMaximum( d->m_widget->maximumZoom() );
        d->updateButtons( d->m_navigationUi.zoomSlider->value() );
    }
}

void NavigationWidgetPrivate::updateButtons( int zoom )
{
    if ( zoom <= m_navigationUi.zoomSlider->minimum() ) {
        m_navigationUi.zoomInButton->setEnabled( true );
        m_navigationUi.zoomOutButton->setEnabled( false );
    } else if ( zoom >= m_navigationUi.zoomSlider->maximum() ) {
        m_navigationUi.zoomInButton->setEnabled( false );
        m_navigationUi.zoomOutButton->setEnabled( true );
    } else {
        m_navigationUi.zoomInButton->setEnabled( true );
        m_navigationUi.zoomOutButton->setEnabled( true );
    }
}

void NavigationWidgetPrivate::mapCenterOnSignal( const QModelIndex &index )
{
    if( !index.isValid() ) {
        return;
    }
    GeoDataObject *object
            = qVariantValue<GeoDataObject*>( index.model()->data(index, MarblePlacemarkModel::ObjectPointerRole ) );
    GeoDataPlacemark *placemark = dynamic_cast<GeoDataPlacemark*>( object );
    if ( placemark ) {
        m_widget->centerOn( *placemark, true );
        m_widget->model()->placemarkSelectionModel()->select( m_sortproxy->mapToSource( index ), QItemSelectionModel::ClearAndSelect );
    }
}

void NavigationWidgetPrivate::adjustForAnimation()
{
    // TODO: use signals here as well
    if ( m_widget ) {
        m_widget->setViewContext( Animation );
    }
}

void NavigationWidgetPrivate::adjustForStill()
{
    // TODO: use signals here as well
    if ( m_widget ) {
        m_widget->setViewContext( Still );
    }
}

void NavigationWidget::resizeEvent ( QResizeEvent * )
{
    bool const smallScreen = MarbleGlobal::getInstance()->profiles() & MarbleGlobal::SmallScreen;

    if ( smallScreen || height() < 390 ) {
        if ( !d->m_navigationUi.zoomSlider->isHidden() ) {
            setUpdatesEnabled(false);
            d->m_navigationUi.zoomSlider->hide();
            d->m_navigationUi.m_pSpacerFrame->setSizePolicy( QSizePolicy::Preferred,
                                                             QSizePolicy::Expanding );
            setUpdatesEnabled(true);
        }
    } else {
        if ( d->m_navigationUi.zoomSlider->isHidden() ) {
            setUpdatesEnabled(false);
            d->m_navigationUi.zoomSlider->show();
            d->m_navigationUi.m_pSpacerFrame->setSizePolicy( QSizePolicy::Preferred,
                                                       QSizePolicy::Fixed );
            setUpdatesEnabled(true);
        }
    }
}

}

#include "NavigationWidget.moc"
