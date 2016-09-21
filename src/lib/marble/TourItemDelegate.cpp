//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2013 Mihail Ivchenko <ematirov@gmail.com>
// Copyright 2014 Sanjiban Bairagya <sanjiban22393@gmail.com>
// Copyright 2014 Illya Kovalevskyy <illya.kovalevskyy@gmail.com>
//

#include <QAbstractTextDocumentLayout>
#include <QStyleOptionButton>
#include <QPainter>
#include <QApplication>
#include <QListView>
#include <QPointer>

#include "TourItemDelegate.h"
#include "MarblePlacemarkModel.h"
#include "geodata/data/GeoDataObject.h"
#include "geodata/data/GeoDataTourControl.h"
#include "geodata/data/GeoDataWait.h"
#include "geodata/data/GeoDataCoordinates.h"
#include "geodata/data/GeoDataSoundCue.h"
#include "geodata/data/GeoDataAnimatedUpdate.h"
#include "FlyToEditWidget.h"
#include "TourControlEditWidget.h"
#include "SoundCueEditWidget.h"
#include "WaitEditWidget.h"
#include "RemoveItemEditWidget.h"
#include "GeoDataPlacemark.h"
#include "GeoDataTypes.h"
#include "GeoDataCreate.h"
#include "GeoDataUpdate.h"
#include "GeoDataDelete.h"
#include "GeoDataChange.h"
#include "EditPlacemarkDialog.h"
#include "MarbleWidget.h"
#include "GeoDataPlaylist.h"
#include "TourWidget.h"

namespace Marble
{

TourItemDelegate::TourItemDelegate( QListView* view, MarbleWidget* widget, TourWidget* tour ):
                    m_listView( view ), m_widget( widget ), m_editable( true ), m_tourWidget( tour )
{
    QObject::connect( this, SIGNAL(editingChanged(QModelIndex)), m_listView, SLOT(update(QModelIndex)) );
    m_listView->setEditTriggers( QAbstractItemView::NoEditTriggers );
}

void TourItemDelegate::paint( QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index ) const
{
    QStyleOptionViewItem styleOption = option;
    styleOption.text = QString();
    QApplication::style()->drawControl(QStyle::CE_ItemViewItem, &styleOption, painter);

    QAbstractTextDocumentLayout::PaintContext paintContext;
    if (styleOption.state & QStyle::State_Selected) {
        paintContext.palette.setColor(QPalette::Text,
            styleOption.palette.color(QPalette::Active, QPalette::HighlightedText));
    }

    if ( m_listView->currentIndex() == index && m_tourWidget->isPlaying() ) {
        painter->fillRect( option.rect, paintContext.palette.color( QPalette::Midlight ) );
        QStyledItemDelegate::paint( painter, option, index );
    }

    QTextDocument label;
    QRect const labelRect = position(Label, option);
    label.setTextWidth( labelRect.width() );
    label.setDefaultFont( option.font );

    QStyleOptionButton button;
    button.state = option.state;
    button.palette = option.palette;
    button.features = QStyleOptionButton::None;
    button.iconSize = QSize( 16, 16 );
    button.state &= ~QStyle::State_HasFocus;
    if( !editable() ) {
        button.state &= ~QStyle::State_Enabled;
    }

    QRect const iconRect = position( GeoDataElementIcon, option );

    GeoDataObject *object = qvariant_cast<GeoDataObject*>(index.data( MarblePlacemarkModel::ObjectPointerRole ) );
    if ( object->nodeType() == GeoDataTypes::GeoDataTourControlType && !m_editingIndices.contains( index ) ) {
        GeoDataTourControl *tourControl = static_cast<GeoDataTourControl*> ( object );
        GeoDataTourControl::PlayMode const playMode = tourControl->playMode();

        if ( playMode == GeoDataTourControl::Play ) {
            label.setHtml( tr("Play the tour") );
        } else if ( playMode == GeoDataTourControl::Pause ) {
            label.setHtml( tr("Pause the tour") );
        }
        painter->save();
        painter->translate( labelRect.topLeft() );
        painter->setClipRect( 0, 0, labelRect.width(), labelRect.height() );
        label.documentLayout()->draw( painter, paintContext );
        painter->restore();
        button.icon = QIcon(QStringLiteral(":/marble/document-edit.png"));

        QRect const buttonRect = position( EditButton, option );;
        button.rect = buttonRect;

        QIcon const icon = QIcon(QStringLiteral(":/marble/media-playback-pause.png"));
        painter->drawPixmap( iconRect, icon.pixmap( iconRect.size() ) );

    } else if ( object->nodeType() == GeoDataTypes::GeoDataFlyToType && !m_editingIndices.contains( index ) ) {
        GeoDataCoordinates const flyToCoords = index.data( MarblePlacemarkModel::CoordinateRole ).value<GeoDataCoordinates>();
        label.setHtml( flyToCoords.toString() );
        button.icon = QIcon(QStringLiteral(":/marble/document-edit.png"));

        painter->save();
        painter->translate( labelRect.topLeft() );
        painter->setClipRect( 0, 0, labelRect.width(), labelRect.height() );
        label.documentLayout()->draw( painter, paintContext );
        painter->restore();

        QRect const buttonRect = position( EditButton, option );
        button.rect = buttonRect;

        QIcon const icon = QIcon(QStringLiteral(":/marble/flag.png"));
        painter->drawPixmap( iconRect, icon.pixmap( iconRect.size() ) );

    } else if ( object->nodeType() == GeoDataTypes::GeoDataWaitType && !m_editingIndices.contains( index ) ) {
        GeoDataWait *wait = static_cast<GeoDataWait*> ( object );
        label.setHtml( tr("Wait for %1 seconds").arg( QString::number( wait->duration() ) ) );

        painter->save();
        painter->translate( labelRect.topLeft() );
        painter->setClipRect( 0, 0, labelRect.width(), labelRect.height() );
        label.documentLayout()->draw( painter, paintContext );
        painter->restore();

        button.icon = QIcon(QStringLiteral(":/marble/document-edit.png"));

        QRect const buttonRect = position( EditButton, option );
        button.rect = buttonRect;

        QIcon const icon = QIcon(QStringLiteral(":/marble/player-time.png"));
        painter->drawPixmap( iconRect, icon.pixmap( iconRect.size() ) );

    } else if ( object->nodeType() == GeoDataTypes::GeoDataSoundCueType && !m_editingIndices.contains( index ) ) {
        GeoDataSoundCue *soundCue = static_cast<GeoDataSoundCue*>( object );
        label.setHtml(soundCue->href().section(QLatin1Char('/'), -1));

        painter->save();
        painter->translate( labelRect.topLeft() );
        painter->setClipRect( 0, 0, labelRect.width(), labelRect.height() );
        label.documentLayout()->draw( painter, paintContext );
        painter->restore();

        QStyleOptionButton playButton = button;

        button.icon = QIcon(QStringLiteral(":/marble/document-edit.png"));
        QRect const buttonRect = position( EditButton, option );
        button.rect = buttonRect;

        playButton.icon = QIcon(QStringLiteral(":/marble/playback-play.png"));
        QRect const playButtonRect = position( ActionButton, option );
        playButton.rect = playButtonRect;
        QApplication::style()->drawControl( QStyle::CE_PushButton, &playButton, painter );

        QIcon const icon = QIcon(QStringLiteral(":/marble/audio-x-generic.png"));
        painter->drawPixmap( iconRect, icon.pixmap( iconRect.size() ) );
    } else if ( object->nodeType() == GeoDataTypes::GeoDataAnimatedUpdateType && !m_editingIndices.contains( index ) ){
        GeoDataAnimatedUpdate *animUpdate = static_cast<GeoDataAnimatedUpdate*>( object );
        GeoDataUpdate *update = animUpdate->update();
        QString text;
        bool ok = false;
        QString iconString;
        if( update && update->create() && update->create()->size() != 0
                   && ( update->create()->first().nodeType() == GeoDataTypes::GeoDataFolderType ||
                        update->create()->first().nodeType() == GeoDataTypes::GeoDataDocumentType ) ) {
            GeoDataContainer *container = static_cast<GeoDataContainer*>( update->create()->child( 0 ) );
            if( container->size() > 0 ) {
                label.setHtml( tr( "Create item %1" ).arg( container->first().id() ) );
                ok = true;
                iconString = QStringLiteral(":/icons/add-placemark.png");
            }
        } else if( update && update->getDelete() && update->getDelete()->size() != 0 ){
            label.setHtml( tr( "Remove item %1" ).arg( update->getDelete()->first().targetId() ) );
            ok = true;
            iconString = QStringLiteral(":/icons/remove.png");
        } else if( update && update->change() && update->change()->size() != 0 ){
            label.setHtml( tr( "Change item %1" ).arg( update->change()->first().targetId() ) );
            ok = true;
            iconString = QStringLiteral(":/marble/document-edit.png");
        }
        if( update && !ok ) {
            label.setHtml( tr( "Update items" ) );
            button.state &= ~QStyle::State_Enabled & ~QStyle::State_Sunken;
        }

        painter->save();
        painter->translate( labelRect.topLeft() );
        painter->setClipRect( 0, 0, labelRect.width(), labelRect.height() );
        label.documentLayout()->draw( painter, paintContext );
        painter->restore();

        button.icon = QIcon(QStringLiteral(":/marble/document-edit.png"));
        QRect const buttonRect = position( EditButton, option );
        button.rect = buttonRect;

        QIcon const icon = QIcon( iconString );
        painter->drawPixmap( iconRect, icon.pixmap( iconRect.size() ) );
    }
    QApplication::style()->drawControl( QStyle::CE_PushButton, &button, painter );
}

QRect TourItemDelegate::position( Element element, const QStyleOptionViewItem &option )
{
    QPoint const topCol1 = option.rect.topLeft() + QPoint(10, 10);
    QPoint const topCol2 = topCol1 + QPoint(30, 0);
    QPoint const topCol3 = topCol2 + QPoint(210, 0);
    QPoint const topCol4 = topCol3 + QPoint(30, 0);
    QSize const labelSize = QSize(220, 30);
    QSize const iconsSize = QSize(22, 22);

    switch(element)
    {
    case GeoDataElementIcon:
        return QRect( topCol1, iconsSize );
    case Label:
        return QRect( topCol2, labelSize );
    case EditButton:
        return QRect( topCol3, iconsSize );
    case ActionButton:
        return QRect( topCol4, iconsSize );
    }
    return QRect();
}

QStringList TourItemDelegate::findIds(GeoDataPlaylist *playlist, bool onlyFeatures) const
{
    if( playlist == 0 ) {
        return QStringList();
    }
    QStringList result;
    for( int i = 0; i < playlist->size(); ++i ) {
        GeoDataTourPrimitive *primitive = playlist->primitive( i );
        if( !primitive->id().isEmpty() && !onlyFeatures ) {
            result << primitive->id();
        }
        if( primitive->nodeType() == GeoDataTypes::GeoDataAnimatedUpdateType ) {
            GeoDataAnimatedUpdate *animatedUpdate = static_cast<GeoDataAnimatedUpdate*>( primitive );
            if( animatedUpdate->update() != 0 ) {
                GeoDataUpdate *update = animatedUpdate->update();
                if( !update->id().isEmpty() && !onlyFeatures ) {
                    result << update->id();
                }
                if( update->create() != 0 ) {
                    if( !update->create()->id().isEmpty() && !onlyFeatures ) {
                        result << update->create()->id();
                    }
                    for( int j = 0; j < update->create()->size(); ++j ) {
                        if( !update->create()->at( j ).id().isEmpty() ) {
                            result << update->create()->at( j ).id();
                        }
                    }
                }
                if( update->change() != 0 ) {
                    if( !update->change()->id().isEmpty() && !onlyFeatures ) {
                        result << update->change()->id();
                    }
                    for( int j = 0; j < update->change()->size(); ++j ) {
                        if( !update->change()->at( j ).id().isEmpty() ) {
                            result << update->change()->at( j ).id();
                        }
                    }
                }
                if( update->getDelete() != 0 ) {
                    if( !update->getDelete()->id().isEmpty() && !onlyFeatures ) {
                        result << update->getDelete()->id();
                    }
                    for( int j = 0; j < update->getDelete()->size(); ++j ) {
                        if( !update->getDelete()->at( j ).id().isEmpty() ) {
                            result << update->getDelete()->at( j ).id();
                        }
                    }
                }
            }
        }
    }
    return result;
}

GeoDataPlaylist *TourItemDelegate::playlist() const
{
    QModelIndex const rootIndex = m_listView->rootIndex();
    if( rootIndex.isValid() ) {
        GeoDataObject *rootObject = static_cast<GeoDataObject*>( rootIndex.internalPointer() );
        if ( rootObject->nodeType() == GeoDataTypes::GeoDataPlaylistType ) {
            GeoDataPlaylist *playlist = static_cast<GeoDataPlaylist*>( rootObject );
            return playlist;
        }
    }
    return 0;
}


QSize TourItemDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED( option );
    Q_UNUSED( index );
    return QSize(290,50);
}

QWidget* TourItemDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    Q_UNUSED( option );
    GeoDataObject *object = qvariant_cast<GeoDataObject*>(index.data( MarblePlacemarkModel::ObjectPointerRole ) );
    if ( object->nodeType() == GeoDataTypes::GeoDataFlyToType ) {
        FlyToEditWidget* widget = new FlyToEditWidget(index, m_widget, parent);
        widget->setFirstFlyTo( m_firstFlyTo );
        connect(widget, SIGNAL(editingDone(QModelIndex)), this, SLOT(closeEditor(QModelIndex)));
        connect( this, SIGNAL(editableChanged(bool)), widget, SLOT(setEditable(bool)) );
        connect( this, SIGNAL(firstFlyToChanged(QPersistentModelIndex)), widget, SLOT(setFirstFlyTo(QPersistentModelIndex)) );
        return widget;

    } else if ( object->nodeType() == GeoDataTypes::GeoDataTourControlType ) {
        TourControlEditWidget* widget = new TourControlEditWidget(index, parent);
        connect(widget, SIGNAL(editingDone(QModelIndex)), this, SLOT(closeEditor(QModelIndex)));
        connect( this, SIGNAL(editableChanged(bool)), widget, SLOT(setEditable(bool)) );
        return widget;

    } else if ( object->nodeType() == GeoDataTypes::GeoDataWaitType ) {
        WaitEditWidget* widget = new WaitEditWidget(index, parent);
        connect(widget, SIGNAL(editingDone(QModelIndex)), this, SLOT(closeEditor(QModelIndex)));
        connect( this, SIGNAL(editableChanged(bool)), widget, SLOT(setEditable(bool)) );
        return widget;

    } else if ( object->nodeType() == GeoDataTypes::GeoDataSoundCueType ) {
        SoundCueEditWidget* widget = new SoundCueEditWidget(index, parent);
        connect(widget, SIGNAL(editingDone(QModelIndex)), this, SLOT(closeEditor(QModelIndex)));
        connect( this, SIGNAL(editableChanged(bool)), widget, SLOT(setEditable(bool)) );
        return widget;

    } else if ( object->nodeType() == GeoDataTypes::GeoDataAnimatedUpdateType ) {
        RemoveItemEditWidget* widget = new RemoveItemEditWidget(index, parent);
        GeoDataPlaylist *playlistObject = playlist();
        if( playlistObject != 0 ) {
            widget->setFeatureIds( findIds( playlistObject ) );
        }
        widget->setDefaultFeatureId( m_defaultFeatureId );
        connect(widget, SIGNAL(editingDone(QModelIndex)), this, SLOT(closeEditor(QModelIndex)));
        connect( this, SIGNAL(editableChanged(bool)), widget, SLOT(setEditable(bool)) );
        connect( this, SIGNAL(featureIdsChanged(QStringList)), widget, SLOT(setFeatureIds(QStringList)) );
        connect( this, SIGNAL(defaultFeatureIdChanged(QString)), widget, SLOT(setDefaultFeatureId(QString)) );
        return widget;

    }
    return 0;
}

bool TourItemDelegate::editable() const
{
    return m_editable;
}

void TourItemDelegate::setEditable( bool editable )
{
    if( m_editable != editable ) {
        m_editable = editable;
        emit editableChanged( m_editable );
    }
}

QModelIndex TourItemDelegate::firstFlyTo() const
{
    return m_firstFlyTo;
}

bool TourItemDelegate::editAnimatedUpdate(GeoDataAnimatedUpdate *animatedUpdate, bool create)
{
    if( animatedUpdate->update() == 0 ) {
        return false;
    }
    GeoDataFeature *feature = 0;
    if( create && !( animatedUpdate->update()->create() == 0 || animatedUpdate->update()->create()->size() == 0 ) ) {
        GeoDataContainer *container = dynamic_cast<GeoDataContainer*>( animatedUpdate->update()->create()->child( 0 ) );
        if( container != 0 && container->size() ) {
            feature = container->child( 0 );
        }
    } else if ( !create && !( animatedUpdate->update()->change() == 0 || animatedUpdate->update()->change()->size() == 0 ) ) {
        GeoDataContainer *container = dynamic_cast<GeoDataContainer*>( animatedUpdate->update()->change()->child( 0 ) );
        if( container != 0 && container->size() ) {
            feature = container->child( 0 );
        }
    }
    if( feature == 0 ) {
        return false;
    }

    QStringList ids;

    GeoDataPlacemark *placemark = static_cast<GeoDataPlacemark*>( feature );

    if( !create ) {
        if( placemark->targetId().isEmpty() && !defaultFeatureId().isEmpty() ) {
            GeoDataFeature *feature = findFeature( defaultFeatureId() );
            if( feature != 0 && feature->nodeType() == GeoDataTypes::GeoDataPlacemarkType ) {
                GeoDataPlacemark *targetPlacemark = static_cast<GeoDataPlacemark*>( feature );
                animatedUpdate->update()->change()->placemarkList().remove( 0 );
                delete placemark;
                placemark = new GeoDataPlacemark( *targetPlacemark );
                animatedUpdate->update()->change()->placemarkList().insert( 0, placemark );
                placemark->setTargetId( defaultFeatureId() );
                placemark->setId(QString());
            }
        }
    }

    QPointer<EditPlacemarkDialog> dialog = new EditPlacemarkDialog( placemark, nullptr, m_widget );
    if( create ) {
        dialog->setWindowTitle( QObject::tr( "Add Placemark to Tour" ) );
    } else {
        dialog->setWindowTitle( QObject::tr( "Change Placemark in Tour" ) );
        dialog->setTargetIdFieldVisible( true );
        dialog->setIdFieldVisible( false );
    }
    GeoDataPlaylist* playlistObject = playlist();
    if( playlistObject != 0 ) {
        ids.append( findIds( playlistObject, true ) );
    }
    ids.removeOne( placemark->id() );
    if( create ) {
        dialog->setIdFilter( ids );
    } else {
        dialog->setTargetIds( ids );
    }
    bool status = dialog->exec();
    if( !create ) {
        placemark->setId(QString());
    }
    return status;
}

QString TourItemDelegate::defaultFeatureId() const
{
    return m_defaultFeatureId;
}



GeoDataFeature *TourItemDelegate::findFeature(const QString &id) const
{
    GeoDataPlaylist *playlistObject = playlist();
    if( playlistObject == 0 ) {
        return 0;
    }
    GeoDataFeature *result = 0;
    for( int i = 0; i < playlistObject->size(); ++i ) {
        GeoDataTourPrimitive *primitive = playlistObject->primitive( i );
        if( primitive->nodeType() == GeoDataTypes::GeoDataAnimatedUpdateType ) {
            GeoDataAnimatedUpdate *animatedUpdate = static_cast<GeoDataAnimatedUpdate*>( primitive );
            if( animatedUpdate->update() != 0 ) {
                GeoDataUpdate *update = animatedUpdate->update();
                if( update->create() != 0 ) {
                    for( int j = 0; j < update->create()->featureList().size(); ++j ) {
                        if( update->create()->at( j ).id() == id ) {
                            result = update->create()->featureList().at( j );
                        }
                    }
                }
                if( update->change() != 0 ) {
                    for( int j = 0; j < update->change()->featureList().size(); ++j ) {
                        if( update->change()->at( j ).id() == id ) {
                            result = update->change()->featureList().at( j );
                        }
                    }
                }
                if( update->getDelete() != 0 ) {
                    for( int j = 0; j < update->getDelete()->featureList().size(); ++j ) {
                        if( update->getDelete()->at( j ).id() == id ) {
                            result = update->getDelete()->featureList().at( j );
                        }
                    }
                }
            }
        }
    }
    return result;
}

void TourItemDelegate::setFirstFlyTo(const QPersistentModelIndex &index )
{
    m_firstFlyTo = index;
    emit firstFlyToChanged( m_firstFlyTo );
}

void TourItemDelegate::setDefaultFeatureId(const QString &id)
{
    m_defaultFeatureId = id;
    QStringList ids = findIds( playlist() );
    emit featureIdsChanged( ids );
    emit defaultFeatureIdChanged( id );
}

bool TourItemDelegate::editorEvent( QEvent* event, QAbstractItemModel* model, const QStyleOptionViewItem& option, const QModelIndex& index )
{
    Q_UNUSED( model );
    if ( ( event->type() == QEvent::MouseButtonRelease ) && editable() ) {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent*>( event );
        QRect editRect = position( EditButton, option );
        if ( editRect.contains( mouseEvent->pos() ) ) {
            if( m_editingIndices.contains( index ) ) {
                m_editingIndices.removeOne( index );
                emit editingChanged( index );
                return true;
            }else{
                GeoDataObject *object = qvariant_cast<GeoDataObject*>(index.data( MarblePlacemarkModel::ObjectPointerRole ) );
                if( object->nodeType() == GeoDataTypes::GeoDataAnimatedUpdateType ) {
                    GeoDataAnimatedUpdate *animatedUpdate = static_cast<GeoDataAnimatedUpdate*>( object );
                    if( animatedUpdate->update() && animatedUpdate->update()->create() ) {
                        if( editAnimatedUpdate( animatedUpdate ) ) {
                            setDefaultFeatureId( m_defaultFeatureId );
                        }
                    } else if( animatedUpdate->update() && animatedUpdate->update()->change() ) {
                        editAnimatedUpdate( animatedUpdate, false );
                    } else if ( animatedUpdate->update() && animatedUpdate->update()->getDelete() ) {
                        m_editingIndices.append( index );
                        m_listView->openPersistentEditor( index );
                    }
                } else {
                    m_editingIndices.append( index );
                    m_listView->openPersistentEditor( index );
                }
            }
            emit editingChanged( index );
            return true;
        }
    }
    return false;
}

void TourItemDelegate::closeEditor( const QModelIndex &index )
{
    emit edited( index );
    m_listView->closePersistentEditor( index );
    m_editingIndices.removeOne( index );
}

}

#include "moc_TourItemDelegate.cpp"
