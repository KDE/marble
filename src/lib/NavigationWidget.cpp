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
#include "GeoDataDocument.h"
#include "GeoDataTreeModel.h"
#include "GeoSceneDocument.h"
#include "GeoSceneHead.h"

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
    Ui::NavigationWidget    m_navigationUi;
    MarbleWidget           *m_widget;
    QSortFilterProxyModel  *m_sortproxy;
    QString                 m_searchTerm;
    MarbleRunnerManager    *m_runnerManager;
    QTimer                  m_deferSearch;
    GeoDataTreeModel        m_treeModel;
    GeoDataDocument        *m_document;

    NavigationWidgetPrivate()
        : m_document( new GeoDataDocument ) {
    m_document->setDocumentRole( SearchResultDocument );
    m_treeModel.setRootDocument( m_document );
    };

};

NavigationWidget::NavigationWidget( QWidget *parent, Qt::WindowFlags f )
    : QWidget( parent, f ),
      d( new NavigationWidgetPrivate() )
{
    d->m_searchTerm.clear();
    d->m_widget = 0;

    d->m_navigationUi.setupUi( this );

    d->m_sortproxy = new QSortFilterProxyModel( this );
    d->m_navigationUi.locationListView->setModel( d->m_sortproxy );
    d->m_deferSearch.setSingleShot( true );
    connect ( &d->m_deferSearch, SIGNAL( timeout() ),
              this, SLOT( search()) );

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

    connect( d->m_navigationUi.searchLineEdit,   SIGNAL( textChanged( const QString& ) ),
             this,                               SLOT( searchLineChanged( const QString& ) ) );
    connect( d->m_navigationUi.searchLineEdit,   SIGNAL( returnPressed() ),
             this,                               SLOT( searchReturnPressed() ) );

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
    d->m_runnerManager = new MarbleRunnerManager( widget->model()->pluginManager(), this );
    connect( d->m_runnerManager, SIGNAL( searchResultChanged( QVector<GeoDataPlacemark*> ) ),
             this,               SLOT( setLocations( QVector<GeoDataPlacemark*> ) ) );

    d->m_widget = widget;
    d->m_runnerManager->setModel( widget->model() );
    d->m_widget->model()->treeModel()->addDocument( d->m_document );

    d->m_sortproxy->setSortLocaleAware( true );
    d->m_sortproxy->setDynamicSortFilter( true );

    // Make us aware of all the Placemarks in the MarbleModel so that
    // we can search them.
    d->m_sortproxy->setSourceModel( d->m_widget->model()->placemarkModel() );
    d->m_sortproxy->sort( 0 );

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
    // As we have disabled all zoomSlider Signals, we have to update our buttons seperately.
    updateButtons( zoom );

    d->m_navigationUi.zoomSlider->blockSignals( false );
}

void NavigationWidget::searchLineChanged( const QString &search )
{
    d->m_searchTerm = search;
    // if search line is empty, restore original geonames
    if ( d->m_searchTerm.isEmpty() ) {
        // set the proxy list to the placemarkModel
        d->m_sortproxy->setSourceModel( d->m_widget->model()->placemarkModel() );
        d->m_sortproxy->sort( 0 );
        d->m_widget->model()->placemarkSelectionModel()->clear();

        // clear the local document
        d->m_widget->model()->treeModel()->removeDocument( d->m_document );
        d->m_document->clear();
        d->m_widget->model()->treeModel()->addDocument( d->m_document );
    }
    d->m_deferSearch.start( 500 );
}

void NavigationWidget::searchReturnPressed()
{
    // do nothing if search term empty
    if ( !d->m_searchTerm.isEmpty() ) {
        d->m_runnerManager->findPlacemarks( d->m_searchTerm );
    }
}

void NavigationWidget::search()
{
    d->m_deferSearch.stop();
    int  currentSelected = d->m_navigationUi.locationListView->currentIndex().row();
    d->m_navigationUi.locationListView->selectItem( d->m_searchTerm );
    if ( currentSelected != d->m_navigationUi.locationListView->currentIndex().row() )
        d->m_navigationUi.locationListView->activate();
}

void NavigationWidget::setLocations( QVector<GeoDataPlacemark*> locations )
{
    QTime t;
    t.start();

    // fill the local document with results
    d->m_widget->model()->placemarkSelectionModel()->clear();
    d->m_widget->model()->treeModel()->removeDocument( d->m_document );
    d->m_document->clear();
    foreach (GeoDataPlacemark *placemark, locations ) {
        d->m_document->append( new GeoDataPlacemark( *placemark ) );
    }
    d->m_widget->model()->treeModel()->addDocument( d->m_document );

    // set the proxy list to the list of results
    d->m_sortproxy->setSourceModel( &d->m_treeModel );
    d->m_sortproxy->sort( 0 );
    mDebug() << "NavigationWidget (sort): Time elapsed:"<< t.elapsed() << " ms";
}

void NavigationWidget::selectTheme( const QString &theme )
{
    Q_UNUSED( theme )

    if( !d->m_widget )
        return;

    d->m_navigationUi.zoomSlider->setMinimum( d->m_widget->minimumZoom() );
    d->m_navigationUi.zoomSlider->setMaximum( d->m_widget->maximumZoom() );
    updateButtons( d->m_navigationUi.zoomSlider->value() );
}

void NavigationWidget::updateButtons( int value )
{
    if ( value <= d->m_navigationUi.zoomSlider->minimum() ) {
        d->m_navigationUi.zoomInButton->setEnabled( true );
        d->m_navigationUi.zoomOutButton->setEnabled( false );
    } else if ( value >= d->m_navigationUi.zoomSlider->maximum() ) {
        d->m_navigationUi.zoomInButton->setEnabled( false );
        d->m_navigationUi.zoomOutButton->setEnabled( true );
    } else {
        d->m_navigationUi.zoomInButton->setEnabled( true );
        d->m_navigationUi.zoomOutButton->setEnabled( true );
    }
}

void NavigationWidget::mapCenterOnSignal( const QModelIndex &index )
{
    if( !index.isValid() ) {
        return;
    }
    GeoDataObject *object
            = qVariantValue<GeoDataObject*>( index.model()->data(index, MarblePlacemarkModel::ObjectPointerRole ) );
    if ( dynamic_cast<GeoDataPlacemark*>(object) )
    {
        GeoDataPlacemark *placemark = static_cast<GeoDataPlacemark*>(object);
        d->m_widget->centerOn( *placemark, true );
        d->m_widget->model()->placemarkSelectionModel()->select( d->m_sortproxy->mapToSource( index ), QItemSelectionModel::ClearAndSelect );
    }
}

void NavigationWidget::adjustForAnimation()
{
    // TODO: use signals here as well
    if ( !d->m_widget )
        return;

    d->m_widget->setViewContext( Animation );
}

void NavigationWidget::adjustForStill()
{
    // TODO: use signals here as well
    if ( !d->m_widget )
        return;

    d->m_widget->setViewContext( Still );
}

void NavigationWidget::resizeEvent ( QResizeEvent * )
{
    bool smallScreen = MarbleGlobal::getInstance()->profiles() & MarbleGlobal::SmallScreen;

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
