//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2013 Mihail Ivchenko <ematirov@gmail>
// Copyright 2014 Sanjiban Bairagya <sanjiban22393@gmail.com>
//

#include "TourWidget.h"
#include "FlyToEditWidget.h"
#include "TourControlEditWidget.h"
#include "WaitEditWidget.h"
#include "SoundCueEditWidget.h"
#include "TourItemDelegate.h"

#include "ui_TourWidget.h"
#include "GeoDataPlacemark.h"
#include "GeoDataDocument.h"
#include "GeoDataLookAt.h"
#include "GeoDataPlaylist.h"
#include "GeoDataTour.h"
#include "GeoDataTreeModel.h"
#include "GeoDataTypes.h"
#include "GeoDataFlyTo.h"
#include "GeoDataWait.h"
#include "GeoDataCamera.h"
#include "GeoDataTourControl.h"
#include "GeoDataSoundCue.h"
#include "GeoDataCreate.h"
#include "GeoDataUpdate.h"
#include "GeoDataDelete.h"
#include "GeoDataChange.h"
#include "GeoDataAnimatedUpdate.h"
#include "GeoDataDocumentWriter.h"
#include "KmlElementDictionary.h"
#include "MarbleModel.h"
#include "MarblePlacemarkModel.h"
#include "MarbleWidget.h"
#include "ParsingRunnerManager.h"
#include "TourPlayback.h"
#include "MovieCapture.h"
#include "TourCaptureDialog.h"
#include "MarbleDebug.h"
#include "PlaybackFlyToItem.h"
#include "EditPlacemarkDialog.h"
#include "MarbleDirs.h"
#include "GeoDataStyle.h"
#include "GeoDataIconStyle.h"

#include <QFileDialog>
#include <QDir>
#include <QModelIndex>
#include <QMessageBox>
#include <QPainter>
#include <QToolButton>
#include <QMenu>
#include <QUrl>
#include <QKeyEvent>
#include <QCloseEvent>
#include <QPointer>

namespace Marble
{

class TourWidgetPrivate
{

public:
    explicit TourWidgetPrivate( TourWidget *parent );
    GeoDataFeature *getPlaylistFeature() const;
    void updateRootIndex();

public:
    void openFile();
    bool openFile( const QString &filename );
    void createTour();
    void saveTour();
    void saveTourAs();
    void mapCenterOn(const QModelIndex &index );
    void addFlyTo();
    void addWait();
    void addSoundCue();
    void addPlacemark();
    void addRemovePlacemark();
    void addChangePlacemark();
    void addTourPrimitive(GeoDataTourPrimitive *primitive );
    void deleteSelected();
    void updateButtonsStates();
    void moveUp();
    void moveDown();
    void captureTour();
    void handlePlaybackProgress( const double position );
    void handlePlaybackFinish();
    GeoDataObject *rootIndexObject() const;

private:
    GeoDataTour* findTour( GeoDataFeature* feature ) const;
    bool openDocument( GeoDataDocument *document );
    bool saveTourAs( const QString &filename );
    bool overrideModifications();

public:
    TourWidget *q;
    MarbleWidget *m_widget;
    Ui::TourWidget  m_tourUi;
    TourCaptureDialog *m_tourCaptureDialog;
    TourPlayback m_playback;
    TourItemDelegate *m_delegate;
    bool m_isChanged;
    bool m_playState;
    bool m_isLoopingStopped;
    GeoDataDocument* m_document;
    QAction *m_actionToggleLoopPlay;
    QToolButton *m_addPrimitiveButton;
    QAction *m_actionAddFlyTo;
    QAction *m_actionAddWait;
    QAction *m_actionAddSoundCue;
    QAction *m_actionAddPlacemark;
    QAction *m_actionAddRemovePlacemark;
    QAction *m_actionAddChangePlacemark;
};

TourWidgetPrivate::TourWidgetPrivate( TourWidget *parent )
    : q( parent ),
      m_widget( 0 ),
      m_playback( 0 ),
      m_delegate( 0 ),
      m_isChanged( false ),
      m_playState( false ),
      m_document( 0 ),
      m_addPrimitiveButton( new QToolButton )
{
    m_tourUi.setupUi( parent );
    m_tourUi.m_actionRecord->setEnabled( false );

    QAction *separator = m_tourUi.m_toolBarControl->insertSeparator( m_tourUi.m_actionMoveUp );

    m_addPrimitiveButton->setIcon(QIcon(QStringLiteral(":/marble/flag.png")));
    m_addPrimitiveButton->setToolTip( QObject::tr( "Add FlyTo" ) );
    m_addPrimitiveButton->setPopupMode( QToolButton::MenuButtonPopup );

    QMenu *addPrimitiveMenu = new QMenu;

    m_actionAddFlyTo = new QAction(QIcon(QStringLiteral(":/marble/flag.png")), QObject::tr("Add FlyTo"), addPrimitiveMenu);
    addPrimitiveMenu->addAction( m_actionAddFlyTo );
    m_actionAddWait = new QAction(QIcon(QStringLiteral(":/marble/player-time.png")), QObject::tr("Add Wait"), addPrimitiveMenu);
    addPrimitiveMenu->addAction( m_actionAddWait );
    m_actionAddSoundCue = new QAction(QIcon(QStringLiteral(":/marble/audio-x-generic.png")), QObject::tr("Add SoundCue"), addPrimitiveMenu);
    addPrimitiveMenu->addAction( m_actionAddSoundCue );
    addPrimitiveMenu->addSeparator();
    m_actionAddPlacemark = new QAction(QIcon(QStringLiteral(":/icons/add-placemark.png")), QObject::tr("Add Placemark"), addPrimitiveMenu);
    addPrimitiveMenu->addAction( m_actionAddPlacemark );
    m_actionAddRemovePlacemark = new QAction(QIcon(QStringLiteral(":/icons/remove.png")), QObject::tr("Remove placemark"), addPrimitiveMenu);
    addPrimitiveMenu->addAction( m_actionAddRemovePlacemark );
    m_actionAddChangePlacemark = new QAction(QIcon(QStringLiteral(":/marble/document-edit.png")), QObject::tr("Change placemark"), addPrimitiveMenu);
    addPrimitiveMenu->addAction( m_actionAddChangePlacemark );
    m_actionToggleLoopPlay = new QAction( QObject::tr( "Loop" ), m_tourUi.m_slider );
    m_actionToggleLoopPlay->setCheckable( true );
    m_actionToggleLoopPlay->setChecked( false );
    m_tourUi.m_slider->setContextMenuPolicy( Qt::ActionsContextMenu );
    m_tourUi.m_slider->addAction( m_actionToggleLoopPlay );

    m_addPrimitiveButton->setMenu( addPrimitiveMenu );
    m_addPrimitiveButton->setEnabled( false );

    m_tourUi.m_toolBarControl->insertWidget( separator, m_addPrimitiveButton );

    QObject::connect( m_tourUi.m_listView, SIGNAL(activated(QModelIndex)), q, SLOT(mapCenterOn(QModelIndex)) );
    QObject::connect( m_addPrimitiveButton, SIGNAL(clicked()), q, SLOT(addFlyTo()) );
    QObject::connect( m_actionAddFlyTo, SIGNAL(triggered()), q, SLOT(addFlyTo()) );
    QObject::connect( m_actionAddWait, SIGNAL(triggered()), q, SLOT(addWait()) );
    QObject::connect( m_actionAddSoundCue, SIGNAL(triggered()), q, SLOT(addSoundCue()) );
    QObject::connect( m_actionAddPlacemark, SIGNAL(triggered()), q, SLOT(addPlacemark()) );
    QObject::connect( m_actionAddRemovePlacemark, SIGNAL(triggered()), q, SLOT(addRemovePlacemark()) );
    QObject::connect( m_actionAddChangePlacemark, SIGNAL(triggered()), q, SLOT(addChangePlacemark()) );
    QObject::connect( m_tourUi.m_actionDelete, SIGNAL(triggered()), q, SLOT(deleteSelected()) );
    QObject::connect( m_tourUi.m_actionMoveUp, SIGNAL(triggered()), q, SLOT(moveUp()) );
    QObject::connect( m_tourUi.m_actionMoveDown, SIGNAL(triggered()), q, SLOT(moveDown()) );
    QObject::connect( m_tourUi.m_actionNewTour, SIGNAL(triggered()), q, SLOT(createTour()) );
    QObject::connect( m_tourUi.m_actionOpenTour, SIGNAL(triggered()), q, SLOT(openFile()) );
    QObject::connect( m_tourUi.m_actionSaveTour, SIGNAL(triggered()), q, SLOT(saveTour()) );
    QObject::connect( m_tourUi.m_actionSaveTourAs, SIGNAL(triggered()), q, SLOT(saveTourAs()) );
    QObject::connect( m_tourUi.m_actionRecord, SIGNAL(triggered()), q, SLOT(captureTour()) );
    QObject::connect( &m_playback, SIGNAL(finished()), q, SLOT(stopPlaying()) );
    QObject::connect( &m_playback, SIGNAL(itemFinished(int)), q, SLOT(setHighlightedItemIndex(int)) );

}

TourWidget::TourWidget( QWidget *parent, Qt::WindowFlags flags )
    : QWidget( parent, flags ),
      d( new TourWidgetPrivate( this ) )
{
    layout()->setMargin( 0 );

    connect( d->m_tourUi.actionPlay, SIGNAL(triggered()),
            this, SLOT(togglePlaying()) );
    connect( d->m_tourUi.actionStop, SIGNAL(triggered()),
            this, SLOT(stopLooping()) );
    connect( d->m_tourUi.actionStop, SIGNAL(triggered()),
            this, SLOT(stopPlaying()) );
    connect( d->m_tourUi.m_slider, SIGNAL(sliderMoved(int)),
             this, SLOT(handleSliderMove(int)) );

    d->m_tourUi.m_toolBarPlayback->setDisabled( true );
    d->m_tourUi.m_slider->setDisabled( true );
    d->m_tourUi.m_listView->installEventFilter( this );
}

TourWidget::~TourWidget()
{
    delete d;
}

bool TourWidget::eventFilter( QObject *watched, QEvent *event )
{
    Q_UNUSED(watched);

    Q_ASSERT( watched == d->m_tourUi.m_listView );
    GeoDataObject *rootObject =  d->rootIndexObject();

    if ( !rootObject ) {
        return false;
    }

    if ( event->type() == QEvent::KeyPress ) {
        QKeyEvent *key = static_cast<QKeyEvent*>( event );
        QModelIndexList selectedIndexes = d->m_tourUi.m_listView->selectionModel()->selectedIndexes();

        if ( key->key() == Qt::Key_Delete ) {
            if ( !selectedIndexes.isEmpty() ) {
                deleteSelected();
            }
            return true;
        }

        if ( key->key() == Qt::Key_PageDown && key->modifiers().testFlag( Qt::ControlModifier )
             && !selectedIndexes.isEmpty() )
        {
            QModelIndexList::iterator end = selectedIndexes.end() - 1;
            if ( rootObject && rootObject->nodeType() == GeoDataTypes::GeoDataPlaylistType ) {
                GeoDataPlaylist *playlist = static_cast<GeoDataPlaylist*>( rootObject );

                if ( end->row() != playlist->size() - 1 ) {
                    moveDown();
                }
            }
            return true;
        }

        if ( key->key() == Qt::Key_PageUp && key->modifiers().testFlag( Qt::ControlModifier )
             && !selectedIndexes.isEmpty() )
        {
            QModelIndexList::iterator start = selectedIndexes.begin();
            if ( start->row() != 0 ) {
                moveUp();
            }
            return true;
        }
    }

    return false;
}

void TourWidget::setMarbleWidget( MarbleWidget *widget )
{
    d->m_widget = widget;
    d->m_delegate = new TourItemDelegate( d->m_tourUi.m_listView, d->m_widget, this );
    connect( d->m_delegate, SIGNAL(edited(QModelIndex)), this, SLOT(updateDuration()) );
    connect( d->m_delegate, SIGNAL(edited(QModelIndex)), &d->m_playback, SLOT(updateTracks()) );
    d->m_tourUi.m_listView->setItemDelegate( d->m_delegate );
}

void TourWidget::togglePlaying()
{
    if( !d->m_playState ){
        d->m_playState = true;
        startPlaying();
    } else {
        d->m_playState = false;
        pausePlaying();
    }
}

void TourWidget::startPlaying()
{
    setHighlightedItemIndex( 0 );
    d->m_isLoopingStopped = false;
    d->m_playback.play();
    d->m_tourUi.actionPlay->setIcon(QIcon(QStringLiteral(":/marble/playback-pause.png")));
    d->m_tourUi.actionPlay->setEnabled( true );
    d->m_tourUi.actionStop->setEnabled( true );
    d->m_tourUi.m_actionRecord->setEnabled( false );
    d->m_delegate->setEditable( false );
    d->m_addPrimitiveButton->setEnabled( false );
    d->m_playState = true;
}

void TourWidget::pausePlaying()
{
    d->m_playback.pause();
    d->m_tourUi.actionPlay->setIcon(QIcon(QStringLiteral(":/marble/playback-play.png")));
    d->m_tourUi.actionPlay->setEnabled( true );
    d->m_tourUi.actionStop->setEnabled( true );
}

void TourWidget::stopPlaying()
{
    removeHighlight();
    d->m_playback.stop();
    d->m_tourUi.actionPlay->setIcon(QIcon(QStringLiteral(":/marble/playback-play.png")));
    d->m_tourUi.actionPlay->setEnabled( true );
    d->m_tourUi.m_actionRecord->setEnabled( true );
    d->m_tourUi.actionStop->setEnabled( false );
    d->m_playState = false;
    d->m_delegate->setEditable( true );
    d->m_addPrimitiveButton->setEnabled( true );

    // Loop if the option ( m_actionLoopPlay ) is checked
    if ( d->m_actionToggleLoopPlay->isChecked() && !d->m_isLoopingStopped ) {
        startPlaying();
    }
}

void TourWidget::stopLooping()
{
    d->m_isLoopingStopped = true;
}

void TourWidget::closeEvent( QCloseEvent *event )
{
    if ( !d->m_document || !d->m_isChanged ) {
        event->accept();
        return;
    }

    const int result = QMessageBox::question( d->m_widget,
                                             QObject::tr( "Save tour" ),
                                             QObject::tr( "There are unsaved Tours. Do you want to save your changes?" ),
                                             QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel );

    switch ( result ) {
    case QMessageBox::Save:
        d->saveTour();
        event->accept();
        break;
    case QMessageBox::Discard:
        event->accept();
        break;
    case QMessageBox::Cancel:
        event->ignore();
    }
}

void TourWidget::handleSliderMove( int value )
{
    removeHighlight();
    d->m_playback.seek( value / 100.0 );
    QTime nullTime( 0, 0, 0 );
    QTime time = nullTime.addSecs(  value / 100.0 );
    d->m_tourUi.m_elapsedTime->setText(QString("%L1:%L2").arg(time.minute(), 2, 10, QLatin1Char('0')).arg(time.second(), 2, 10, QLatin1Char('0')));
}

void TourWidgetPrivate::openFile()
{
    if ( overrideModifications() ) {
        QString const filename = QFileDialog::getOpenFileName( q, QObject::tr( "Open Tour" ), QDir::homePath(), QObject::tr( "KML Tours (*.kml)" ) );
        if ( !filename.isEmpty() ) {
            ParsingRunnerManager manager( m_widget->model()->pluginManager() );
            GeoDataDocument* document = manager.openFile( filename );
            m_playback.setBaseUrl( QUrl::fromLocalFile( filename ) );
            openDocument( document );
        }
    }
}

bool TourWidgetPrivate::openFile( const QString &filename )
{
    if ( overrideModifications() ) {
        if ( !filename.isEmpty() ) {
            ParsingRunnerManager manager( m_widget->model()->pluginManager() );
            GeoDataDocument* document = manager.openFile( filename );
            m_playback.setBaseUrl( QUrl::fromLocalFile( filename ) );
            return openDocument( document );
        }
    }

    return false;
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
    QVariant coordinatesVariant = m_widget->model()->treeModel()->data( index, MarblePlacemarkModel::CoordinateRole );
    if ( !coordinatesVariant.isNull() ) {
        GeoDataCoordinates const coordinates = coordinatesVariant.value<GeoDataCoordinates>();
        GeoDataLookAt lookat;
        lookat.setCoordinates( coordinates );
        lookat.setRange( coordinates.altitude() );
        m_widget->flyTo( lookat, Instant );
    }
}

void TourWidgetPrivate::addFlyTo()
{
    GeoDataFlyTo *flyTo = new GeoDataFlyTo();
    GeoDataLookAt *lookat = new GeoDataLookAt( m_widget->lookAt() );
    lookat->setAltitude( lookat->range() );
    flyTo->setView( lookat );
    bool isMainTrackEmpty = m_playback.mainTrackSize() == 0;
    flyTo->setDuration( isMainTrackEmpty ? 0.0 : 1.0 );
    addTourPrimitive( flyTo );
}

void TourWidgetPrivate::addWait()
{
    GeoDataWait *wait = new GeoDataWait();
    wait->setDuration( 1.0 );
    addTourPrimitive( wait );
}

void TourWidgetPrivate::addSoundCue()
{
    GeoDataSoundCue *soundCue = new GeoDataSoundCue();
    addTourPrimitive( soundCue );
}

void TourWidgetPrivate::addPlacemark()
{
    // Get the normalized coordinates of the focus point. There will be automatically added a new
    // placemark.
    qreal lat = m_widget->focusPoint().latitude();
    qreal lon = m_widget->focusPoint().longitude();
    GeoDataCoordinates::normalizeLonLat( lon, lat );

    GeoDataDocument *document = new GeoDataDocument;
    if( m_document->id().isEmpty() ) {
        if( m_document->name().isEmpty() ) {
            m_document->setId(QStringLiteral("untitled_tour"));
        } else {
            m_document->setId( m_document->name().trimmed().replace( QLatin1Char(' '), QLatin1Char('_') ).toLower() );
        }
    }
    document->setTargetId( m_document->id() );

    GeoDataPlacemark *placemark = new GeoDataPlacemark;
    placemark->setCoordinate( lon, lat );
    placemark->setVisible( true );
    placemark->setBalloonVisible( true );
    GeoDataStyle *newStyle = new GeoDataStyle( *placemark->style() );
    newStyle->iconStyle().setIconPath(MarbleDirs::path(QStringLiteral("bitmaps/redflag_22.png")));
    placemark->setStyle( GeoDataStyle::Ptr(newStyle) );

    document->append( placemark );

    GeoDataCreate *create = new GeoDataCreate;
    create->append( document );
    GeoDataUpdate *update = new GeoDataUpdate;
    update->setCreate( create );
    GeoDataAnimatedUpdate *animatedUpdate = new GeoDataAnimatedUpdate;
    animatedUpdate->setUpdate( update );

    if( m_delegate->editAnimatedUpdate( animatedUpdate ) ) {
        addTourPrimitive( animatedUpdate );
        m_delegate->setDefaultFeatureId( placemark->id() );
    } else {
        delete animatedUpdate;
    }
}

void TourWidgetPrivate::addRemovePlacemark()
{
    GeoDataDelete *deleteItem = new GeoDataDelete;
    GeoDataPlacemark *placemark = new GeoDataPlacemark;
    placemark->setTargetId( m_delegate->defaultFeatureId() );
    deleteItem->append( placemark );
    GeoDataUpdate *update = new GeoDataUpdate;
    update->setDelete( deleteItem );
    GeoDataAnimatedUpdate *animatedUpdate = new GeoDataAnimatedUpdate;
    animatedUpdate->setUpdate( update );
    addTourPrimitive( animatedUpdate );
}

void TourWidgetPrivate::addChangePlacemark()
{
    GeoDataChange *change = new GeoDataChange;
    GeoDataPlacemark *placemark = 0;
    GeoDataFeature *lastFeature = m_delegate->findFeature( m_delegate->defaultFeatureId() );
    if( lastFeature != 0 && lastFeature->nodeType() == GeoDataTypes::GeoDataPlacemarkType ) {
        GeoDataPlacemark *target = static_cast<GeoDataPlacemark*>( lastFeature );
        placemark = new GeoDataPlacemark( *target );
        placemark->setTargetId( m_delegate->defaultFeatureId() );
        placemark->setId(QString());
    } else {
        placemark = new GeoDataPlacemark;
    }
    change->append( placemark );
    GeoDataUpdate *update = new GeoDataUpdate;
    update->setChange( change );
    GeoDataAnimatedUpdate *animatedUpdate = new GeoDataAnimatedUpdate;
    animatedUpdate->setUpdate( update );
    addTourPrimitive( animatedUpdate );
}

void TourWidgetPrivate::addTourPrimitive( GeoDataTourPrimitive *primitive )
{
    GeoDataObject *rootObject =  rootIndexObject();
    if ( rootObject->nodeType() == GeoDataTypes::GeoDataPlaylistType ) {
        GeoDataPlaylist *playlist = static_cast<GeoDataPlaylist*>( rootObject );
        QModelIndex currentIndex = m_tourUi.m_listView->currentIndex();
        QModelIndex playlistIndex = m_widget->model()->treeModel()->index( playlist );
        int row = currentIndex.isValid() ? currentIndex.row()+1 : playlist->size();
        m_widget->model()->treeModel()->addTourPrimitive( playlistIndex, primitive, row );
        m_isChanged = true;
        m_tourUi.m_actionSaveTour->setEnabled( true );

        // Scrolling to the inserted item.
        if ( currentIndex.isValid() ) {
            m_tourUi.m_listView->scrollTo( currentIndex );
        }
        else {
            m_tourUi.m_listView->scrollToBottom();
        }
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
            QModelIndex playlistIndex = m_widget->model()->treeModel()->index( playlist );
            QModelIndexList selected = m_tourUi.m_listView->selectionModel()->selectedIndexes();
            qSort( selected.begin(), selected.end(), qGreater<QModelIndex>() );
            QModelIndexList::iterator end = selected.end();
            QModelIndexList::iterator iter = selected.begin();
            for( ; iter != end; ++iter ) {
                m_widget->model()->treeModel()->removeTourPrimitive( playlistIndex, iter->row() );
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
        QModelIndex playlistIndex = m_widget->model()->treeModel()->index( playlist );
        QModelIndexList selected = m_tourUi.m_listView->selectionModel()->selectedIndexes();
        qSort( selected.begin(), selected.end(), qLess<QModelIndex>() );
        QModelIndexList::iterator end = selected.end();
        QModelIndexList::iterator iter = selected.begin();
        for( ; iter != end; ++iter ) {
            int const index = iter->row();
            Q_ASSERT( index > 0 );
            m_widget->model()->treeModel()->swapTourPrimitives( playlistIndex, index-1, index );
        }
        m_isChanged = true;
        m_tourUi.m_actionSaveTour->setEnabled( true );
        updateButtonsStates();
    }
}

void TourWidgetPrivate::moveDown()
{
    GeoDataObject *rootObject = rootIndexObject();
    if ( rootObject && rootObject->nodeType() == GeoDataTypes::GeoDataPlaylistType ) {
        GeoDataPlaylist *playlist = static_cast<GeoDataPlaylist*>( rootObject );
        QModelIndex playlistIndex = m_widget->model()->treeModel()->index( playlist );
        QModelIndexList selected = m_tourUi.m_listView->selectionModel()->selectedIndexes();
        qSort( selected.begin(), selected.end(), qGreater<QModelIndex>() );
        QModelIndexList::iterator end = selected.end();
        QModelIndexList::iterator iter = selected.begin();
        for( ; iter != end; ++iter ) {
            int const index = iter->row();
            Q_ASSERT( index < playlist->size()-1 );
            m_widget->model()->treeModel()->swapTourPrimitives( playlistIndex, index, index+1 );
        }
        m_isChanged = true;
        m_tourUi.m_actionSaveTour->setEnabled( true );
        updateButtonsStates();
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
    GeoDataTour *tour = findTour( m_document );
    if ( tour ){
        GeoDataPlaylist *playlist = tour->playlist();
        if ( playlist ) {
            m_tourUi.m_listView->setModel( m_widget->model()->treeModel() );
            m_tourUi.m_listView->setRootIndex( m_widget->model()->treeModel()->index( playlist ) );
            QObject::connect( m_tourUi.m_listView->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
                              q, SLOT(updateButtonsStates()) );
        }
        m_playback.setMarbleWidget( m_widget );
        m_playback.setTour( tour );
        m_tourUi.m_slider->setMaximum( m_playback.duration() * 100 );
        QTime nullTime( 0, 0, 0 );
        QTime time = nullTime.addSecs( m_playback.duration() );
        m_tourUi.m_totalTime->setText(QString("%L1:%L2").arg(time.minute(), 2, 10, QLatin1Char('0')).arg(time.second(), 2, 10, QLatin1Char('0')));
        QObject::connect( &m_playback, SIGNAL(progressChanged(double)),
                         q, SLOT(handlePlaybackProgress(double)) );
        q->stopPlaying();
        m_tourUi.m_toolBarPlayback->setEnabled( true );
        bool isPlaybackEmpty = m_playback.mainTrackSize() != 0;
        m_tourUi.actionPlay->setEnabled( isPlaybackEmpty );
        m_tourUi.m_slider->setEnabled( isPlaybackEmpty );
        m_tourUi.m_actionRecord->setEnabled( isPlaybackEmpty );
        m_tourUi.actionStop->setEnabled( false );
        if( m_playback.mainTrackSize() > 0 ) {
            if( dynamic_cast<PlaybackFlyToItem*>( m_playback.mainTrackItemAt( 0 ) ) ) {
                QModelIndex playlistIndex = m_widget->model()->treeModel()->index( playlist );
                for( int i = 0; i < playlist->size(); ++i ) {
                    if( playlist->primitive( i )->nodeType() == GeoDataTypes::GeoDataFlyToType ) {
                        m_delegate->setFirstFlyTo( m_widget->model()->treeModel()->index( i, 0, playlistIndex ) );
                        break;
                    }
                }
            } else {
                m_delegate->setFirstFlyTo( QPersistentModelIndex() );
            }
        }
    }
}

void TourWidget::addFlyTo()
{
    d->addFlyTo();
    finishAddingItem();
}

void TourWidget::addWait()
{
    d->addWait();
    finishAddingItem();
}

void TourWidget::addSoundCue()
{
    d->addSoundCue();
    finishAddingItem();
}

void TourWidget::addPlacemark()
{
    d->addPlacemark();
    finishAddingItem();
}

void TourWidget::addRemovePlacemark()
{
    d->addRemovePlacemark();
    finishAddingItem();
}

void TourWidget::addChangePlacemark()
{
    d->addChangePlacemark();
    finishAddingItem();
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

void TourWidget::updateDuration()
{
    d->m_tourUi.m_slider->setMaximum( d->m_playback.duration() * 100 );
    QTime nullTime( 0, 0, 0 );
    QTime totalTime = nullTime.addSecs( d->m_playback.duration() );
    d->m_tourUi.m_totalTime->setText(QString("%L1:%L2").arg(totalTime.minute(), 2, 10, QLatin1Char('0') ).arg(totalTime.second(), 2, 10, QLatin1Char('0')));
    d->m_tourUi.m_slider->setValue( 0 );
    d->m_tourUi.m_elapsedTime->setText(QString("%L1:%L2").arg(0, 2, 10, QLatin1Char('0')).arg(0, 2, 10, QLatin1Char('0')));
}

void TourWidget::finishAddingItem()
{
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
        document->setName(QStringLiteral("New Tour"));
        document->setId(QStringLiteral("new_tour"));
        GeoDataTour *tour = new GeoDataTour();
        tour->setName(QStringLiteral("New Tour"));
        GeoDataPlaylist *playlist = new GeoDataPlaylist;
        tour->setPlaylist( playlist );
        document->append( static_cast<GeoDataFeature*>( tour ) );
        m_playback.setBaseUrl( QUrl::fromLocalFile( MarbleDirs::marbleDataPath() ) );
        openDocument( document );
        m_isChanged = true;
        m_tourUi.m_actionSaveTour->setEnabled( true );
        m_tourUi.m_slider->setEnabled( true );
    }
}

bool TourWidgetPrivate::openDocument(GeoDataDocument* document)
{
    if ( document ) {
        if ( m_document ) {
            m_widget->model()->treeModel()->removeDocument( m_document );
            delete m_document;
        }
        m_document = document;
        m_widget->model()->treeModel()->addDocument( m_document );
        m_isChanged = false;
        updateRootIndex();
        m_addPrimitiveButton->setEnabled( true );
        m_tourUi.m_actionSaveTourAs->setEnabled( true );
        m_tourUi.m_actionSaveTour->setEnabled( false );
        m_isChanged = false;
        return true;
    }
    return false;
}

void TourWidgetPrivate::saveTour()
{
    if ( m_document ) {
        if ( !m_document->fileName().isEmpty() ) {
            saveTourAs( m_document->fileName() );
        } else {
            saveTourAs();
        }
    }
}

void TourWidgetPrivate::saveTourAs()
{
   if ( m_document )
   {
       QString const filename = QFileDialog::getSaveFileName( q, QObject::tr( "Save Tour as" ), QDir::homePath(), QObject::tr( "KML Tours (*.kml)" ) );
       if ( !filename.isEmpty() ) {
            saveTourAs( filename );
       }
   }
}

bool TourWidgetPrivate::saveTourAs(const QString &filename)
{
    if ( !filename.isEmpty() ) {
        if (GeoDataDocumentWriter::write(filename, *m_document)) {
            m_tourUi.m_actionSaveTour->setEnabled( false );
            m_isChanged = false;
            GeoDataDocument* document = m_document;
            if ( !document->fileName().isNull() ) {
                m_widget->model()->removeGeoData( document->fileName() );
            }
            m_widget->model()->addGeoDataFile( filename );
            m_document->setFileName( filename );
            return true;
        }
    }
    return false;
}

void TourWidgetPrivate::captureTour()
{
    MarbleWidget* widget = new MarbleWidget;
    widget->setMapThemeId( m_widget->mapThemeId() );
    widget->resize( 1280, 720 );

    m_widget->model()->treeModel()->removeDocument(m_document);
    widget->model()->treeModel()->addDocument(m_document);

    GeoDataTour* tour = findTour( m_document );
    TourPlayback* playback = new TourPlayback;
    playback->setMarbleWidget( widget );
    playback->setTour( tour );

    m_tourUi.m_listView->setModel( widget->model()->treeModel() );
    if( tour ){
        m_tourUi.m_listView->setRootIndex( widget->model()->treeModel()->index( tour->playlist() ) );
        m_tourUi.m_listView->repaint();

        QPointer<TourCaptureDialog> tourCaptureDialog = new TourCaptureDialog( widget, m_widget );
        tourCaptureDialog->setDefaultFilename( tour->name() );
        tourCaptureDialog->setTourPlayback( playback );
        tourCaptureDialog->exec();
    }

    delete playback;
    widget->model()->treeModel()->removeDocument(m_document);
    m_widget->model()->treeModel()->addDocument(m_document);
    updateRootIndex();
    delete widget;
}

bool TourWidgetPrivate::overrideModifications()
{
    if ( m_document && m_isChanged ) {
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

bool TourWidget::openTour( const QString &filename)
{
    return d->openFile( filename );
}

void TourWidgetPrivate::handlePlaybackProgress(const double position)
{
    if( !m_tourUi.m_slider->isSliderDown() ){
        m_tourUi.m_slider->setValue( position * 100 );
        QTime nullTime( 0, 0, 0 );
        QTime time = nullTime.addSecs( position );
        m_tourUi.m_elapsedTime->setText(QString("%L1:%L2").arg(time.minute(), 2, 10, QLatin1Char('0')).arg(time.second(), 2, 10, QLatin1Char('0')));
    }
}

void TourWidget::setHighlightedItemIndex( int index )
{
    GeoDataObject* rootObject =  d->rootIndexObject();
    GeoDataPlaylist* playlist = static_cast<GeoDataPlaylist*>( rootObject );
    QModelIndex playlistIndex = d->m_widget->model()->treeModel()->index( playlist );

    // Only flyTo and wait items have duration, so the other types have to be skipped.
    int searchedIndex = 0;
    for ( int  i = 0; i < playlist->size(); i++ ) {

        QModelIndex currentIndex = d->m_widget->model()->treeModel()->index( i, 0, playlistIndex );
        GeoDataObject* object = qvariant_cast<GeoDataObject*>(currentIndex.data( MarblePlacemarkModel::ObjectPointerRole ) );

        if ( object->nodeType() == GeoDataTypes::GeoDataFlyToType
          || object->nodeType() == GeoDataTypes::GeoDataWaitType )
                ++searchedIndex;

        if ( index == searchedIndex ) {
            d->m_tourUi.m_listView->selectionModel()->setCurrentIndex( currentIndex, QItemSelectionModel::NoUpdate );
            d->m_tourUi.m_listView->scrollTo( currentIndex );
            break;
        }
    }
    d->m_tourUi.m_listView->viewport()->update();
}

void TourWidget::removeHighlight()
{
    QModelIndex index;

    // Restoring the CurrentIndex to the previously selected item
    // or clearing it if there was no selected item.
    if ( d->m_tourUi.m_listView->selectionModel()->hasSelection() ) {
        index = d->m_tourUi.m_listView->selectionModel()->selectedIndexes().last();
    }
    else {
        index = QModelIndex();
    }

    d->m_tourUi.m_listView->selectionModel()->setCurrentIndex( index, QItemSelectionModel::NoUpdate );
    d->m_tourUi.m_listView->viewport()->update();
}

bool TourWidget::isPlaying() const
{
    return d->m_playState;
}

}

#include "moc_TourWidget.cpp"
