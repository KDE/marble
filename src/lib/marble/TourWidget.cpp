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
#include "GeoWriter.h"
#include "KmlElementDictionary.h"
#include "MarbleModel.h"
#include "MarblePlacemarkModel.h"
#include "MarbleWidget.h"
#include "ParsingRunnerManager.h"
#include "TourPlayback.h"

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

public:
    void openFile();
    void createTour();
    void saveTour();
    void saveTourAs();
    void mapCenterOn(const QModelIndex &index );
    void addFlyTo();
    void deleteSelected();
    void updateButtonsStates();
    void moveUp();
    void moveDown();

private:
    GeoDataTour* findTour( GeoDataFeature* feature ) const;
    GeoDataObject *rootIndexObject() const;
    bool openDocument( GeoDataDocument *document );
    bool saveTourAs( const QString &filename );
    bool overrideModifications();

    bool m_isChanged;

public:
    TourWidget *q;
    MarbleWidget *m_widget;
    Ui::TourWidget  m_tourUi;
    GeoDataTreeModel m_model;
    TourPlayback m_playback;
};

TourWidgetPrivate::TourWidgetPrivate( TourWidget *parent )
    : m_isChanged( false ),
      q( parent ),
      m_widget( 0 ),
      m_playback( 0 )
{
    m_tourUi.setupUi( parent );
    m_tourUi.m_listView->setModel( &m_model );
    m_tourUi.m_listView->setModelColumn(1);

    QObject::connect( m_tourUi.m_listView, SIGNAL( activated( QModelIndex ) ), q, SLOT( mapCenterOn( QModelIndex ) ) );
    QObject::connect( m_tourUi.m_listView->selectionModel(), SIGNAL( selectionChanged( QItemSelection, QItemSelection ) ),
                      q, SLOT( updateButtonsStates() ) );
    QObject::connect( m_tourUi.m_actionAddFlyTo, SIGNAL( triggered() ), q, SLOT( addFlyTo() ) );
    QObject::connect( m_tourUi.m_actionDelete, SIGNAL( triggered() ), q, SLOT( deleteSelected() ) );
    QObject::connect( m_tourUi.m_actionMoveUp, SIGNAL( triggered() ), q, SLOT( moveUp() ) );
    QObject::connect( m_tourUi.m_actionMoveDown, SIGNAL( triggered() ), q, SLOT( moveDown() ) );
    QObject::connect( q, SIGNAL( featureUpdated( GeoDataFeature* ) ), &m_model, SLOT( updateFeature( GeoDataFeature* ) ) );
    QObject::connect( m_tourUi.m_actionNewTour, SIGNAL( triggered() ), q, SLOT( createTour() ) );
    QObject::connect( m_tourUi.m_actionOpenTour, SIGNAL( triggered() ), q, SLOT( openFile() ) );
    QObject::connect( m_tourUi.m_actionSaveTour, SIGNAL( triggered() ), q, SLOT( saveTour() ) );
    QObject::connect( m_tourUi.m_actionSaveTourAs, SIGNAL( triggered() ), q, SLOT( saveTourAs() ) );
}

TourWidget::TourWidget( QWidget *parent, Qt::WindowFlags flags )
    : QWidget( parent, flags ),
      d( new TourWidgetPrivate( this ) )
{
    layout()->setMargin( 0 );

    connect(d->m_tourUi.actionPlay, SIGNAL(triggered()),
            this, SLOT(startPlaying()));
    connect(d->m_tourUi.actionPause, SIGNAL(triggered()),
            this, SLOT(pausePlaying()));
    connect(d->m_tourUi.actionStop, SIGNAL(triggered()),
            this, SLOT(stopPlaying()));

    d->m_tourUi.m_toolBarPlayback->setDisabled(true);
}

TourWidget::~TourWidget()
{
    delete d;
}

void TourWidget::setMarbleWidget( MarbleWidget *widget )
{
    if (d->m_widget) {
        disconnect(&d->m_playback, SIGNAL(centerOn(GeoDataCoordinates)), d->m_widget, SLOT(centerOn(GeoDataCoordinates)));
    }

    d->m_widget = widget;

    if (d->m_widget) {
        connect(&d->m_playback, SIGNAL(centerOn(GeoDataCoordinates)), d->m_widget, SLOT(centerOn(GeoDataCoordinates)));
    }
}

void TourWidget::startPlaying()
{
    d->m_playback.play();

    d->m_tourUi.actionPlay->setEnabled(false);
    d->m_tourUi.actionPause->setEnabled(true);
    d->m_tourUi.actionStop->setEnabled(true);
}

void TourWidget::pausePlaying()
{
    d->m_playback.pause();

    d->m_tourUi.actionPlay->setEnabled(true);
    d->m_tourUi.actionPause->setEnabled(false);
    d->m_tourUi.actionStop->setEnabled(true);
}

void TourWidget::stopPlaying()
{
    d->m_playback.stop();

    d->m_tourUi.actionPlay->setEnabled(true);
    d->m_tourUi.actionPause->setEnabled(false);
    d->m_tourUi.actionStop->setEnabled(false);
}

void TourWidgetPrivate::openFile()
{
    if ( overrideModifications() ) {
        QString const filename = QFileDialog::getOpenFileName( q, QObject::tr( "Open Tour" ), QDir::homePath(), QObject::tr( "KML Tours (*.kml)" ) );
        if ( !filename.isEmpty() ) {
            ParsingRunnerManager manager( m_widget->model()->pluginManager() );
            GeoDataDocument* document = manager.openFile( filename );
            openDocument( document );
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
            playlist->insertPrimitive( currentIndex.row()+1, flyTo );
        } else {
            playlist->addPrimitive( flyTo );
        }
        m_isChanged = true;
        m_tourUi.m_actionSaveTour->setEnabled( true );
    }
}

void TourWidgetPrivate::deleteSelected()
{
    QString title = QObject::tr( "Remove Selected Items" );
    QString text = QObject::tr( "Are you sure want to remove selected items?" );
    QPointer<QMessageBox> dialog = new QMessageBox( QMessageBox::Question, title, text, QMessageBox::Yes | QMessageBox::No, q );
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
                playlist->removePrimitiveAt( iter->row() );
            }
            m_isChanged = true;
            m_tourUi.m_actionSaveTour->setEnabled( true );
        }
    }
    delete dialog;
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
        m_tourUi.m_actionMoveUp->setEnabled( ( start->row() != 0 ) ); // if we can move up enable action else disable.
        GeoDataObject *rootObject =  rootIndexObject();
        if ( rootObject && rootObject->nodeType() == GeoDataTypes::GeoDataPlaylistType ) {
            GeoDataPlaylist *playlist = static_cast<GeoDataPlaylist*>( rootObject );
            m_tourUi.m_actionMoveDown->setEnabled( ( end->row() != playlist->size()-1 ) ); // if we can move down enable action else disable.
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
            int const index = iter->row();
            Q_ASSERT( index > 0 );
            playlist->swapPrimitives( index-1, index );
        }
        m_isChanged = true;
        m_tourUi.m_actionSaveTour->setEnabled( true );
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
            int const index = iter->row();
            Q_ASSERT( index < playlist->size()-1 );
            playlist->swapPrimitives( index, index+1 );
        }
        m_isChanged = true;
        m_tourUi.m_actionSaveTour->setEnabled( true );
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
        m_playback.setTour( tour );
        GeoDataPlaylist *playlist = tour->playlist();
        if ( playlist ) {
            m_tourUi.m_listView->setRootIndex( m_model.index( playlist ) );
        }

        q->stopPlaying();
        m_tourUi.m_toolBarPlayback->setEnabled(true);
        m_tourUi.actionPlay->setEnabled(true);
        m_tourUi.actionPause->setEnabled(false);
        m_tourUi.actionStop->setEnabled(false);
    }
}

void TourWidget::addFlyTo()
{
    d->addFlyTo();
    GeoDataFeature *feature = d->getPlaylistFeature();
    if ( feature ) {
        emit featureUpdated( feature );
        d->updateRootIndex();
    }
}

void TourWidget::deleteSelected()
{
    d->deleteSelected();
    GeoDataFeature *feature = d->getPlaylistFeature();
    if ( feature ) {
        emit featureUpdated( feature );
        d->updateRootIndex();
    }
}

void TourWidget::moveDown()
{
    d->moveDown();
    GeoDataFeature *feature = d->getPlaylistFeature();
    if ( feature ) {
        emit featureUpdated( feature );
        d->updateRootIndex();
    }
}

void TourWidget::moveUp()
{
    d->moveUp();
    GeoDataFeature *feature = d->getPlaylistFeature();
    if ( feature ) {
        emit featureUpdated( feature );
        d->updateRootIndex();
    }
}

GeoDataObject *TourWidgetPrivate::rootIndexObject() const
{
    QModelIndex const rootIndex = m_tourUi.m_listView->rootIndex();
    return rootIndex.isValid() ? static_cast<GeoDataObject*>( rootIndex.internalPointer() ) : 0;
}

void TourWidgetPrivate::createTour()
{
    if ( overrideModifications() ) {
        GeoDataDocument *document = new GeoDataDocument();
        document->setDocumentRole( UserDocument );
        document->setName( "New Tour" );
        GeoDataTour *tour = new GeoDataTour();
        tour->setName( "New Tour" );
        GeoDataPlaylist *playlist = new GeoDataPlaylist;
        tour->setPlaylist( playlist );
        document->append( static_cast<GeoDataFeature*>( tour ) );
        openDocument( document );
        m_isChanged = true;
        m_tourUi.m_actionSaveTour->setEnabled( true );
    }
}

bool TourWidgetPrivate::openDocument(GeoDataDocument* document)
{
    if ( document ) {
        GeoDataDocument* oldDocument = m_model.rowCount() ? m_model.rootDocument() : 0;
        if ( oldDocument ) {
            m_widget->model()->removeGeoData( oldDocument->fileName() );
        }
        if ( !document->fileName().isEmpty() ) {
            m_widget->model()->addGeoDataFile( document->fileName() );
        }
        m_model.setRootDocument( document );
        m_isChanged = false;
        updateRootIndex();
        m_tourUi.m_actionAddFlyTo->setEnabled( true );
        m_tourUi.m_actionSaveTourAs->setEnabled( true );
        m_tourUi.m_actionSaveTour->setEnabled( false );
        m_isChanged = false;
        delete oldDocument;
        return true;
    }
    return false;
}

void TourWidgetPrivate::saveTour()
{
    if ( m_model.rowCount() ) {
        if ( !m_model.rootDocument()->fileName().isEmpty() ) {
            saveTourAs( m_model.rootDocument()->fileName() );
        } else {
            saveTourAs();
        }
    }
}

void TourWidgetPrivate::saveTourAs()
{
   if ( m_model.rowCount() )
   {
       QString const filename = QFileDialog::getSaveFileName( q, QObject::tr( "Save Tour as" ), QDir::homePath(), QObject::tr( "KML Tours (*.kml)" ) );
       if ( !filename.isEmpty() ) {
            saveTourAs( filename );
       }
   }
}

bool TourWidgetPrivate::saveTourAs(const QString &filename)
{
    if ( !filename.isEmpty() )
    {
        QFile file( filename );
        if ( file.open( QIODevice::WriteOnly ) ) {
            GeoWriter writer;
            writer.setDocumentType( kml::kmlTag_nameSpace22 );
            if ( writer.write( &file, m_model.rootDocument() ) ) {
                file.close();
                m_tourUi.m_actionSaveTour->setEnabled( false );
                m_isChanged = false;
                GeoDataDocument* document = m_model.rootDocument();
                if ( !document->fileName().isNull() ) {
                    m_widget->model()->removeGeoData( document->fileName() );
                }
                m_widget->model()->addGeoDataFile( filename );
                m_model.rootDocument()->setFileName( filename );
                return true;
            }
        }
    }
    return false;
}

bool TourWidgetPrivate::overrideModifications()
{
    GeoDataDocument* oldDocument = m_model.rowCount() ? m_model.rootDocument() : 0;
    if ( oldDocument && m_isChanged ) {
        QString title = QObject::tr( "Discard Changes" );
        QString text = QObject::tr( "Are you sure want to discard all unsaved changes and close current document?" );
        QPointer<QMessageBox> dialog = new QMessageBox( QMessageBox::Question, title, text, QMessageBox::Yes | QMessageBox::No, q );
        dialog->setDefaultButton( QMessageBox::No );
        if ( dialog->exec() != QMessageBox::Yes ) {
            delete dialog;
	    return false;
        }
        delete dialog;
    }
    return true;
}

}

#include "TourWidget.moc"
