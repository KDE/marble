//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2012       Dennis Nienhüser <earthwings@gentoo.org>
//

#include "SearchWidget.h"
#include "SearchInputWidget.h"
#include "MarbleRunnerManager.h"
#include "MarbleWidget.h"
#include "MarbleModel.h"
#include "BranchFilterProxyModel.h"
#include "MarblePlacemarkModel.h"
#include "ViewportParams.h"
#include "MarbleDebug.h"

#include <QtGui/QListView>
#include <QtGui/QVBoxLayout>

namespace Marble {

class SearchWidgetPrivate
{
public:
    MarbleRunnerManager* m_runnerManager;
    QListView *m_searchResultView;
    MarbleWidget *m_widget;
    BranchFilterProxyModel  m_branchfilter;
    QSortFilterProxyModel  *m_sortproxy;
    QString                 m_searchTerm;
    GeoDataDocument        *m_document;

    SearchWidgetPrivate();
    void setSearchResult( QVector<GeoDataPlacemark*> );
    void search( const QString &searchTerm, SearchMode searchMode );
    void clearSearch();
    void centerMapOn( const QModelIndex &index );
};

SearchWidgetPrivate::SearchWidgetPrivate() :
    m_runnerManager( 0 ),
    m_searchResultView( 0 ),
    m_widget( 0 ),
    m_sortproxy( 0 ),
    m_document( new GeoDataDocument )
{
    m_document->setName( m_widget->tr( "Search Results" ) );
}

void SearchWidgetPrivate::setSearchResult( QVector<GeoDataPlacemark *> locations )
{
    if( locations.isEmpty() ) {
        return;
    }

    QTime timer;
    timer.start();

    // fill the local document with results
    m_widget->model()->placemarkSelectionModel()->clear();
    GeoDataTreeModel *treeModel = m_widget->model()->treeModel();
    treeModel->removeDocument( m_document );
    m_document->clear();
    m_document->setName( QString( m_widget->tr( "Search for '%1'" ) ).arg( m_searchTerm ) );
    foreach (GeoDataPlacemark *placemark, locations ) {
        m_document->append( new GeoDataPlacemark( *placemark ) );
    }
    treeModel->addDocument( m_document );
    m_branchfilter.setBranchIndex( treeModel, treeModel->index( m_document ) );
    m_searchResultView->setRootIndex(
                m_sortproxy->mapFromSource(
                    m_branchfilter.mapFromSource( treeModel->index( m_document ) ) ) );
    m_widget->centerOn( m_document->latLonAltBox() );
    mDebug() << Q_FUNC_INFO << " Time elapsed:"<< timer.elapsed() << " ms";
}

SearchWidget::SearchWidget( QWidget *parent, Qt::WindowFlags flags ) :
    QWidget( parent, flags ),
    d( new SearchWidgetPrivate )
{
    // nothing to do
}

SearchWidget::~SearchWidget()
{
    delete d;
}

void SearchWidget::setMarbleWidget( MarbleWidget* widget )
{
    if ( d->m_widget ) {
        return;
    }

    d->m_widget = widget;

    SearchInputWidget* searchField = new SearchInputWidget;
    setFocusProxy( searchField );
    searchField->setCompletionModel( widget->model()->placemarkModel() );
    connect( searchField, SIGNAL( search( QString, SearchMode ) ), this, SLOT( search( QString, SearchMode ) ) );
    connect( searchField, SIGNAL( centerOn( const GeoDataCoordinates &) ),
             widget, SLOT( centerOn( const GeoDataCoordinates & ) ) );

    d->m_runnerManager = new MarbleRunnerManager( widget->model()->pluginManager(), this );
    connect( d->m_runnerManager, SIGNAL( searchResultChanged( QVector<GeoDataPlacemark*> ) ),
             this,               SLOT( setSearchResult( QVector<GeoDataPlacemark*> ) ) );
    connect( d->m_runnerManager, SIGNAL( searchFinished( QString ) ), searchField, SLOT( disableSearchAnimation() ));

    d->m_runnerManager->setModel( widget->model() );
    GeoDataTreeModel* treeModel = d->m_widget->model()->treeModel();
    treeModel->addDocument( d->m_document );

    d->m_sortproxy = new QSortFilterProxyModel( this );
    d->m_searchResultView = new QListView( this );
    d->m_searchResultView->setModel( d->m_sortproxy );
    d->m_searchResultView->setMinimumSize( 0, 0 );
    connect( d->m_searchResultView, SIGNAL( activated( const QModelIndex& ) ),
             this, SLOT( centerMapOn( const QModelIndex& ) ) );

    d->m_branchfilter.setSourceModel( treeModel );
    d->m_branchfilter.setBranchIndex( treeModel, treeModel->index( d->m_document ) );
    d->m_sortproxy->setSortRole( MarblePlacemarkModel::PopularityIndexRole );
    d->m_sortproxy->sort( 0, Qt::AscendingOrder );
    d->m_sortproxy->setDynamicSortFilter( true );
    d->m_sortproxy->setSourceModel( &d->m_branchfilter );

    d->m_searchResultView->setRootIndex(
                d->m_sortproxy->mapFromSource(
                    d->m_branchfilter.mapFromSource( treeModel->index( d->m_document ) ) ) );

    QVBoxLayout* layout = new QVBoxLayout;
    layout->addWidget( searchField );
    layout->addWidget( d->m_searchResultView );
    layout->setMargin( 0 );
    setLayout( layout );
}

void SearchWidgetPrivate::search( const QString &searchTerm, SearchMode searchMode )
{
    m_searchTerm = searchTerm;

    if( searchTerm.isEmpty() ) {
        clearSearch();
    } else {
        if ( searchMode == AreaSearch ) {
            m_runnerManager->findPlacemarks( m_searchTerm, m_widget->viewport()->viewLatLonAltBox() );
        } else {
            m_runnerManager->findPlacemarks( m_searchTerm );
        }
    }
}

void SearchWidgetPrivate::clearSearch()
{
    m_searchTerm.clear();

    m_widget->model()->placemarkSelectionModel()->clear();

    // clear the local document
    GeoDataTreeModel *treeModel = m_widget->model()->treeModel();
    treeModel->removeDocument( m_document );
    m_document->clear();
    treeModel->addDocument( m_document );
    m_branchfilter.setBranchIndex( treeModel, treeModel->index( m_document ) );
    m_searchResultView->setRootIndex(
            m_sortproxy->mapFromSource(
                m_branchfilter.mapFromSource( treeModel->index( m_document ) ) ) );

    // clear cached search results
    m_runnerManager->findPlacemarks( QString() );
}

void SearchWidgetPrivate::centerMapOn( const QModelIndex &index )
{
    if( !index.isValid() ) {
        return;
    }
    GeoDataObject *object
            = qVariantValue<GeoDataObject*>( index.model()->data(index, MarblePlacemarkModel::ObjectPointerRole ) );
    GeoDataPlacemark *placemark = dynamic_cast<GeoDataPlacemark*>( object );
    if ( placemark ) {
        m_widget->centerOn( *placemark, true );
        m_widget->model()->placemarkSelectionModel()->select( index, QItemSelectionModel::ClearAndSelect );
    }
}

}

#include "SearchWidget.moc"
