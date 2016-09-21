//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2012       Dennis Nienhüser <nienhueser@kde.org>
//

#include "SearchWidget.h"

#include "GeoDataPlacemark.h"
#include "GeoDataLatLonAltBox.h"
#include "GeoDataDocument.h"
#include "GeoDataTreeModel.h"
#include "SearchInputWidget.h"
#include "MarbleWidget.h"
#include "MarbleModel.h"
#include "BranchFilterProxyModel.h"
#include "MarblePlacemarkModel.h"
#include "SearchRunnerManager.h"
#include "ViewportParams.h"
#include "MarbleDebug.h"

#include <QListView>
#include <QVBoxLayout>

namespace Marble {

class SearchWidgetPrivate
{
public:
    SearchRunnerManager* m_runnerManager;
    SearchInputWidget *m_searchField;
    QListView *m_searchResultView;
    MarbleWidget *m_widget;
    BranchFilterProxyModel  m_branchfilter;
    QSortFilterProxyModel   m_sortproxy;
    GeoDataDocument        *m_document;
    QString m_planetId;

    SearchWidgetPrivate();
    void setSearchResult( const QVector<GeoDataPlacemark*>& );
    void search( const QString &searchTerm, SearchMode searchMode );
    void clearSearch();
    void centerMapOn( const QModelIndex &index );
    void handlePlanetChange();
};

SearchWidgetPrivate::SearchWidgetPrivate() :
    m_runnerManager( 0 ),
    m_searchField( 0 ),
    m_searchResultView( 0 ),
    m_widget( 0 ),
    m_branchfilter(),
    m_sortproxy(),
    m_document( new GeoDataDocument )
{
    m_document->setName( QObject::tr( "Search Results" ) );
}

void SearchWidgetPrivate::setSearchResult( const QVector<GeoDataPlacemark *>& locations )
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
    m_document->setName( QString( QObject::tr( "Search for '%1'" ) ).arg( m_searchField->text() ) );
    foreach (GeoDataPlacemark *placemark, locations ) {
        m_document->append( new GeoDataPlacemark( *placemark ) );
    }
    treeModel->addDocument( m_document );
    m_branchfilter.setBranchIndex( treeModel, treeModel->index( m_document ) );
    m_searchResultView->setRootIndex(
                m_sortproxy.mapFromSource(
                    m_branchfilter.mapFromSource( treeModel->index( m_document ) ) ) );
    m_widget->centerOn( m_document->latLonAltBox() );
    mDebug() << Q_FUNC_INFO << " Time elapsed:"<< timer.elapsed() << " ms";
}

SearchWidget::SearchWidget( QWidget *parent, Qt::WindowFlags flags ) :
    QWidget( parent, flags ),
    d( new SearchWidgetPrivate )
{
    d->m_sortproxy.setSortRole( MarblePlacemarkModel::PopularityIndexRole );
    d->m_sortproxy.sort( 0, Qt::AscendingOrder );
    d->m_sortproxy.setDynamicSortFilter( true );
    d->m_sortproxy.setSourceModel( &d->m_branchfilter );

    d->m_searchField = new SearchInputWidget( this );
    setFocusProxy( d->m_searchField );
    connect( d->m_searchField, SIGNAL(search(QString,SearchMode)),
             this, SLOT(search(QString,SearchMode)) );

    d->m_searchResultView = new QListView( this );
    d->m_searchResultView->setModel( &d->m_sortproxy );
    d->m_searchResultView->setMinimumSize( 0, 0 );
    connect( d->m_searchResultView, SIGNAL(activated(QModelIndex)),
             this, SLOT(centerMapOn(QModelIndex)) );

    QVBoxLayout* layout = new QVBoxLayout;
    layout->addWidget( d->m_searchField );
    layout->addWidget( d->m_searchResultView );
    layout->setMargin( 0 );
    setLayout( layout );
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

    d->m_planetId = widget->model()->planetId();
    connect( widget->model(), SIGNAL(themeChanged(QString)),
             this, SLOT(handlePlanetChange()) );

    d->m_searchField->setCompletionModel( widget->model()->placemarkModel() );
    connect( d->m_searchField, SIGNAL(centerOn(GeoDataCoordinates)),
             widget, SLOT(centerOn(GeoDataCoordinates)) );

    d->m_runnerManager = new SearchRunnerManager( widget->model(), this );
    connect( d->m_runnerManager, SIGNAL(searchResultChanged(QVector<GeoDataPlacemark*>)),
             this,               SLOT(setSearchResult(QVector<GeoDataPlacemark*>)) );
    connect( d->m_runnerManager, SIGNAL(searchFinished(QString)),
             d->m_searchField,   SLOT(disableSearchAnimation()));

    GeoDataTreeModel* treeModel = d->m_widget->model()->treeModel();
    treeModel->addDocument( d->m_document );

    d->m_branchfilter.setSourceModel( treeModel );
    d->m_branchfilter.setBranchIndex( treeModel, treeModel->index( d->m_document ) );

    d->m_searchResultView->setRootIndex(
                d->m_sortproxy.mapFromSource(
                    d->m_branchfilter.mapFromSource( treeModel->index( d->m_document ) ) ) );
}

void SearchWidgetPrivate::search( const QString &searchTerm, SearchMode searchMode )
{
    if( searchTerm.isEmpty() ) {
        clearSearch();
    } else {
        if ( searchMode == AreaSearch ) {
            m_runnerManager->findPlacemarks( searchTerm, m_widget->viewport()->viewLatLonAltBox() );
        } else {
            m_runnerManager->findPlacemarks( searchTerm );
        }
    }
}

void SearchWidgetPrivate::clearSearch()
{
    m_widget->model()->placemarkSelectionModel()->clear();

    // clear the local document
    GeoDataTreeModel *treeModel = m_widget->model()->treeModel();
    treeModel->removeDocument( m_document );
    m_document->clear();
    treeModel->addDocument( m_document );
    m_branchfilter.setBranchIndex( treeModel, treeModel->index( m_document ) );
    m_searchResultView->setRootIndex(
            m_sortproxy.mapFromSource(
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
            = index.model()->data(index, MarblePlacemarkModel::ObjectPointerRole ).value<GeoDataObject*>();
    GeoDataPlacemark *placemark = dynamic_cast<GeoDataPlacemark*>( object );
    if ( placemark ) {
        m_widget->centerOn( *placemark, true );
        m_widget->model()->placemarkSelectionModel()->select( index, QItemSelectionModel::ClearAndSelect );
    }
}

void SearchWidgetPrivate::handlePlanetChange()
{
    const QString newPlanetId = m_widget->model()->planetId();

    if (newPlanetId == m_planetId) {
        return;
    }

    m_planetId = newPlanetId;
    clearSearch();
}

}

#include "moc_SearchWidget.cpp"
