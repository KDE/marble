//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2013 Mihail Ivchenko <ematirov@gmail>
//

#include "TourWidget.h"

#include "ui_TourWidget.h"
#include "GeoDataDocument.h"
#include "GeoDataTour.h"
#include "GeoDataTreeModel.h"
#include "GeoDataTypes.h"
#include "GeoDataFlyTo.h"
#include "MarbleModel.h"
#include "MarblePlacemarkModel.h"
#include "ParsingRunnerManager.h"
#include "MarbleWidget.h"

#include <QFileDialog>
#include <QDir>
#include <QModelIndex>
#include <QMessageBox>

namespace Marble
{

class TourWidgetPrivate
{

public:
    TourWidgetPrivate( TourWidget *parent );
    GeoDataFeature *getPlaylistFeature() const;
    void updateRootIndex();

public Q_SLOTS:
    void openFile();
    void mapCenterOn(const QModelIndex &index );
    void addFlyTo();
    void deleteSelected();
    void updateButtonsStates();
    void moveUp();
    void moveDown();

private:
    GeoDataTour* findTour( GeoDataFeature* feature ) const;
    GeoDataObject *rootIndexObject() const;

public:
    TourWidget *q;
    Ui::TourWidget  m_tourUi;
    MarbleWidget *m_widget;
    GeoDataTreeModel m_model;
};

TourWidgetPrivate::TourWidgetPrivate( TourWidget *parent )
    : q( parent ),
      m_widget( 0 )
{
    m_tourUi.setupUi( parent );
    m_tourUi.m_listView->setModel( &m_model );
    m_tourUi.m_listView->setModelColumn(1);
    QObject::connect( m_tourUi.m_openButton, SIGNAL( clicked() ), q, SLOT( openFile() ) );
    QObject::connect( m_tourUi.m_listView, SIGNAL( activated( QModelIndex ) ), q, SLOT( mapCenterOn( QModelIndex ) ) );
    QObject::connect( m_tourUi.m_listView->selectionModel(), SIGNAL( selectionChanged( QItemSelection, QItemSelection ) ),
                      q, SLOT( updateButtonsStates() ) );
    QObject::connect( m_tourUi.m_actionAddFlyTo, SIGNAL( activated() ), q, SLOT( addFlyTo() ) );
    QObject::connect( m_tourUi.m_actionDelete, SIGNAL( activated() ), q, SLOT( deleteSelected() ) );
    QObject::connect( m_tourUi.m_actionMoveUp, SIGNAL( activated() ), q, SLOT( moveUp() ) );
    QObject::connect( m_tourUi.m_actionMoveDown, SIGNAL( activated() ), q, SLOT( moveDown() ) );
    QObject::connect( q, SIGNAL( featureUpdated( GeoDataFeature* ) ), &m_model, SLOT( updateFeature( GeoDataFeature* ) ) );
}

TourWidget::TourWidget( QWidget *parent, Qt::WindowFlags flags )
    : QWidget( parent, flags ),
      d( new TourWidgetPrivate( this ) )
{
    layout()->setMargin( 0 );
}

TourWidget::~TourWidget()
{
    delete d;
}

void TourWidget::setMarbleWidget( MarbleWidget *widget )
{
    d->m_widget = widget;
}

void TourWidgetPrivate::openFile()
{
    QString const filename = QFileDialog::getOpenFileName( q, q->tr( "Open Tour" ), QDir::homePath(), q->tr( "KML Tours (*.kml)" ) );
    if ( !filename.isEmpty() ) {
        m_widget->model()->addGeoDataFile( filename );
        ParsingRunnerManager manager( m_widget->model()->pluginManager() );
        GeoDataDocument* document = manager.openFile( filename );
        if ( document ) {
            m_model.addDocument( document );
            updateRootIndex();
            m_tourUi.m_actionAddFlyTo->setEnabled( true );
        }
    }
}

GeoDataTour *TourWidgetPrivate::findTour( GeoDataFeature *feature ) const
{
    if ( feature && feature->nodeType() == GeoDataTypes::GeoDataTourType ) {
        return static_cast<GeoDataTour*>( feature );
    }

    GeoDataContainer *container = dynamic_cast<GeoDataContainer*>( feature );
    if ( container ) {
        QVector<GeoDataFeature*>::Iterator end = container->end();
        QVector<GeoDataFeature*>::Iterator iter = container->begin();
        for( ; iter != end; ++iter ) {
            GeoDataTour *tour = findTour( *iter );
            if ( tour ) {
                return tour;
            }
        }
    }
    return 0;
}

void TourWidgetPrivate::mapCenterOn( const QModelIndex &index )
{
    QVariant coordinatesVariant = m_model.data( index, MarblePlacemarkModel::CoordinateRole );
    if ( !coordinatesVariant.isNull() ) {
        GeoDataCoordinates const coordinates = coordinatesVariant.value<GeoDataCoordinates>();
        m_widget->centerOn( coordinates );
    }
}

void TourWidgetPrivate::addFlyTo()
{
    GeoDataFlyTo *flyTo = new GeoDataFlyTo();
    GeoDataLookAt *lookAt = new GeoDataLookAt();
    lookAt->setCoordinates( m_widget->focusPoint() );
    flyTo->setView( lookAt );
    GeoDataObject *rootObject =  rootIndexObject();
    if ( rootObject->nodeType() == GeoDataTypes::GeoDataPlaylistType ) {
        GeoDataPlaylist *playlist = static_cast<GeoDataPlaylist*>( rootObject );
        QModelIndex currentIndex = m_tourUi.m_listView->currentIndex();
        if ( currentIndex.isValid() ) {
            playlist->addPrimitive( flyTo, currentIndex.row()+1 );
        } else {
            playlist->addPrimitive( flyTo );
        }
    }
}

void TourWidgetPrivate::deleteSelected()
{
    QString title = q->tr( "Remove Selected Items" );
    QString text = q->tr( "Are you sure want to remove selected items?" );
    QMessageBox *dialog = new QMessageBox( QMessageBox::Question, title, text, QMessageBox::Yes | QMessageBox::No, q );
    dialog->setDefaultButton( QMessageBox::No );
    if ( dialog->exec() == QMessageBox::Yes ) {
        GeoDataObject *rootObject =  rootIndexObject();
        if ( rootObject && rootObject->nodeType() == GeoDataTypes::GeoDataPlaylistType ) {
            GeoDataPlaylist *playlist = static_cast<GeoDataPlaylist*>( rootObject );
            QModelIndexList selected = m_tourUi.m_listView->selectionModel()->selectedIndexes();
            qSort( selected.begin(), selected.end(), qGreater<QModelIndex>() );
            QModelIndexList::iterator end = selected.end();
            QModelIndexList::iterator iter = selected.begin();
            for( ; iter != end; ++iter ) {
                playlist->removePrimitive( ( *iter ).row() );
            }
        }
    }
}

void TourWidgetPrivate::updateButtonsStates()
{
    QModelIndexList selectedIndexes = m_tourUi.m_listView->selectionModel()->selectedIndexes();
    if ( selectedIndexes.isEmpty() ) {
        m_tourUi.m_actionDelete->setEnabled( false );
        m_tourUi.m_actionMoveDown->setEnabled( false );
        m_tourUi.m_actionMoveUp->setEnabled( false );
    } else {
        m_tourUi.m_actionDelete->setEnabled( true );
        qSort( selectedIndexes.begin(), selectedIndexes.end(), qLess<QModelIndex>() );
        QModelIndexList::iterator end = selectedIndexes.end()-1;
        QModelIndexList::iterator start = selectedIndexes.begin();
        m_tourUi.m_actionMoveUp->setEnabled( ( ( *start ).row() != 0 ) ); // if we can move up enable action else disable.
        GeoDataObject *rootObject =  rootIndexObject();
        if ( rootObject && rootObject->nodeType() == GeoDataTypes::GeoDataPlaylistType ) {
            GeoDataPlaylist *playlist = static_cast<GeoDataPlaylist*>( rootObject );
            m_tourUi.m_actionMoveDown->setEnabled( ( ( *end ).row() != playlist->size()-1 ) ); // if we can move down enable action else disable.
        }
    }
}

void TourWidgetPrivate::moveUp()
{
    GeoDataObject *rootObject =  rootIndexObject();
    if ( rootObject && rootObject->nodeType() == GeoDataTypes::GeoDataPlaylistType ) {
        GeoDataPlaylist *playlist = static_cast<GeoDataPlaylist*>( rootObject );
        QModelIndexList selected = m_tourUi.m_listView->selectionModel()->selectedIndexes();
        qSort( selected.begin(), selected.end(), qLess<QModelIndex>() );
        QModelIndexList::iterator end = selected.end();
        QModelIndexList::iterator iter = selected.begin();
        for( ; iter != end; ++iter ) {
            playlist->moveUp( ( *iter ).row() );
        }
    }
}

void TourWidgetPrivate::moveDown()
{
    GeoDataObject *rootObject = rootIndexObject();
    if ( rootObject && rootObject->nodeType() == GeoDataTypes::GeoDataPlaylistType ) {
        GeoDataPlaylist *playlist = static_cast<GeoDataPlaylist*>( rootObject );
        QModelIndexList selected = m_tourUi.m_listView->selectionModel()->selectedIndexes();
        qSort( selected.begin(), selected.end(), qGreater<QModelIndex>() );
        QModelIndexList::iterator end = selected.end();
        QModelIndexList::iterator iter = selected.begin();
        for( ; iter != end; ++iter ) {
            playlist->moveDown( ( *iter ).row() );
        }
    }
}

GeoDataFeature* TourWidgetPrivate::getPlaylistFeature() const
{
    GeoDataObject *rootObject = rootIndexObject();
    if ( rootObject && rootObject->nodeType() == GeoDataTypes::GeoDataPlaylistType ) {
        GeoDataPlaylist *playlist = static_cast<GeoDataPlaylist*>( rootObject );
        GeoDataObject *object = playlist->parent();
        if ( object && object->nodeType() == GeoDataTypes::GeoDataTourType ) {
            return static_cast<GeoDataFeature*>( object );
        }
    }
    return 0;
}

void TourWidgetPrivate::updateRootIndex()
{
    GeoDataTour *tour = findTour( m_model.rootDocument() );
    if ( tour ) {
        GeoDataPlaylist *playlist = tour->playlist();
        if ( playlist ) {
            m_tourUi.m_listView->setRootIndex( m_model.index( playlist ) );
        }
    }
}

void TourWidget::addFlyTo()
{
    d->addFlyTo();
    GeoDataFeature *feature = d->getPlaylistFeature();
    if ( feature ) {
        emit featureUpdated( d->getPlaylistFeature() );
        d->updateRootIndex();
    }
}

void TourWidget::deleteSelected()
{
    d->deleteSelected();
    GeoDataFeature *feature = d->getPlaylistFeature();
    if ( feature ) {
        emit featureUpdated( d->getPlaylistFeature() );
        d->updateRootIndex();
    }
}

void TourWidget::moveDown()
{
    d->moveDown();
    GeoDataFeature *feature = d->getPlaylistFeature();
    if ( feature ) {
        emit featureUpdated( d->getPlaylistFeature() );
        d->updateRootIndex();
    }
}

void TourWidget::moveUp()
{
    d->moveUp();
    GeoDataFeature *feature = d->getPlaylistFeature();
    if ( feature ) {
        emit featureUpdated( d->getPlaylistFeature() );
        d->updateRootIndex();
    }
}

GeoDataObject *TourWidgetPrivate::rootIndexObject() const
{
    QModelIndex const rootIndex = m_tourUi.m_listView->rootIndex();
    return rootIndex.isValid() ? static_cast<GeoDataObject*>( rootIndex.internalPointer() ) : 0;
}

}

#include "TourWidget.moc"
