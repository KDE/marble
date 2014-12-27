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
#include "GeoDataTypes.h"

namespace Marble
{

TourItemDelegate::TourItemDelegate( QListView* view, MarbleWidget* widget ):
                    m_listView( view ), m_widget( widget ), m_editable( true )
{
    QObject::connect( this, SIGNAL( editingChanged( QModelIndex ) ), m_listView, SLOT( update( QModelIndex ) ) );
    m_listView->setEditTriggers( QAbstractItemView::NoEditTriggers );
}

void TourItemDelegate::paint( QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index ) const
{
    QStyleOptionViewItemV4 styleOption = option;
    styleOption.text = QString();
    QApplication::style()->drawControl(QStyle::CE_ItemViewItem, &styleOption, painter);

    QAbstractTextDocumentLayout::PaintContext paintContext;
    if (styleOption.state & QStyle::State_Selected) {
        paintContext.palette.setColor(QPalette::Text,
            styleOption.palette.color(QPalette::Active, QPalette::HighlightedText));
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
        button.icon = QIcon( ":/marble/document-edit.png" );

        QRect const buttonRect = position( EditButton, option );;
        button.rect = buttonRect;

        QIcon const icon = QIcon( ":/marble/media-playback-pause.png" );
        painter->drawPixmap( iconRect, icon.pixmap( iconRect.size() ) );

    } else if ( object->nodeType() == GeoDataTypes::GeoDataFlyToType && !m_editingIndices.contains( index ) ) {
        GeoDataCoordinates const flyToCoords = index.data( MarblePlacemarkModel::CoordinateRole ).value<GeoDataCoordinates>();
        label.setHtml( flyToCoords.toString() );
        button.icon = QIcon( ":/marble/document-edit.png" );

        painter->save();
        painter->translate( labelRect.topLeft() );
        painter->setClipRect( 0, 0, labelRect.width(), labelRect.height() );
        label.documentLayout()->draw( painter, paintContext );
        painter->restore();

        QRect const buttonRect = position( EditButton, option );
        button.rect = buttonRect;

        QIcon const icon = QIcon( ":/marble/flag.png" );
        painter->drawPixmap( iconRect, icon.pixmap( iconRect.size() ) );

    } else if ( object->nodeType() == GeoDataTypes::GeoDataWaitType && !m_editingIndices.contains( index ) ) {
        GeoDataWait *wait = static_cast<GeoDataWait*> ( object );
        label.setHtml( tr("Wait for %1 seconds").arg( QString::number( wait->duration() ) ) );

        painter->save();
        painter->translate( labelRect.topLeft() );
        painter->setClipRect( 0, 0, labelRect.width(), labelRect.height() );
        label.documentLayout()->draw( painter, paintContext );
        painter->restore();

        button.icon = QIcon( ":/marble/document-edit.png" );

        QRect const buttonRect = position( EditButton, option );
        button.rect = buttonRect;

        QIcon const icon = QIcon( ":/marble/player-time.png" );
        painter->drawPixmap( iconRect, icon.pixmap( iconRect.size() ) );

    } else if ( object->nodeType() == GeoDataTypes::GeoDataSoundCueType && !m_editingIndices.contains( index ) ) {
        GeoDataSoundCue *soundCue = static_cast<GeoDataSoundCue*>( object );
        label.setHtml( soundCue->href().section("/", -1) );

        painter->save();
        painter->translate( labelRect.topLeft() );
        painter->setClipRect( 0, 0, labelRect.width(), labelRect.height() );
        label.documentLayout()->draw( painter, paintContext );
        painter->restore();

        QStyleOptionButton playButton = button;

        button.icon = QIcon( ":/marble/document-edit.png" );
        QRect const buttonRect = position( EditButton, option );
        button.rect = buttonRect;

        playButton.icon = QIcon( ":/marble/playback-play.png" );
        QRect const playButtonRect = position( ActionButton, option );
        playButton.rect = playButtonRect;
        QApplication::style()->drawControl( QStyle::CE_PushButton, &playButton, painter );

        QIcon const icon = QIcon( ":/marble/audio-x-generic.png" );
        painter->drawPixmap( iconRect, icon.pixmap( iconRect.size() ) );
    } else if ( object->nodeType() == GeoDataTypes::GeoDataAnimatedUpdateType ){
        GeoDataAnimatedUpdate *animUpdate = static_cast<GeoDataAnimatedUpdate*>( object );
        GeoDataUpdate *update = animUpdate->update();
        QString text;
        if( update ){
            label.setHtml( tr( "Update items" ) );
        }

        painter->save();
        painter->translate( labelRect.topLeft() );
        painter->setClipRect( 0, 0, labelRect.width(), labelRect.height() );
        label.documentLayout()->draw( painter, paintContext );
        painter->restore();

        button.icon = QIcon( ":/marble/document-edit.png" );
        QRect const buttonRect = position( EditButton, option );
        button.rect = buttonRect;
        button.state &= ~QStyle::State_Enabled & ~QStyle::State_Sunken;

        QIcon const icon = QIcon( ":/marble/player-time.png" );
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
        connect(widget, SIGNAL(editingDone(QModelIndex)), this, SLOT(closeEditor(QModelIndex)));
        connect( this, SIGNAL( editableChanged( bool) ), widget, SLOT( setEditable( bool ) ) );
        return widget;

    } else if ( object->nodeType() == GeoDataTypes::GeoDataTourControlType ) {
        TourControlEditWidget* widget = new TourControlEditWidget(index, parent);
        connect(widget, SIGNAL(editingDone(QModelIndex)), this, SLOT(closeEditor(QModelIndex)));
        connect( this, SIGNAL( editableChanged( bool) ), widget, SLOT( setEditable( bool ) ) );
        return widget;

    } else if ( object->nodeType() == GeoDataTypes::GeoDataWaitType ) {
        WaitEditWidget* widget = new WaitEditWidget(index, parent);
        connect(widget, SIGNAL(editingDone(QModelIndex)), this, SLOT(closeEditor(QModelIndex)));
        connect( this, SIGNAL( editableChanged( bool) ), widget, SLOT( setEditable( bool ) ) );
        return widget;

    } else if ( object->nodeType() == GeoDataTypes::GeoDataSoundCueType ) {
        SoundCueEditWidget* widget = new SoundCueEditWidget(index, parent);
        connect(widget, SIGNAL(editingDone(QModelIndex)), this, SLOT(closeEditor(QModelIndex)));
        connect( this, SIGNAL( editableChanged( bool) ), widget, SLOT( setEditable( bool ) ) );
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
                m_editingIndices.append( index );
                m_listView->openPersistentEditor( index );
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

#include "TourItemDelegate.moc"
