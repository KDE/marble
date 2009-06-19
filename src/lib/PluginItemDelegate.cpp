//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009 Bastian Holst <bastianholst@gmx.de>
//

// Self
#include "PluginItemDelegate.h"

// Marble
#include "RenderPlugin.h"

// Qt
#include <QtCore/QDebug>
#include <QtCore/QEvent>
#include <QtCore/QSize>
#include <QtCore/QVariant>
#include <QtGui/QAbstractItemView>
#include <QtGui/QMouseEvent>
#include <QtGui/QStandardItemModel>
#include <QtGui/QApplication>
#include <QtGui/QPainter>
#include <QtGui/QStandardItem>

using namespace Marble;

PluginItemDelegate::PluginItemDelegate( QObject * parent )
    : QAbstractItemDelegate( parent )
{
}

PluginItemDelegate::~PluginItemDelegate() {
}

void PluginItemDelegate::paint( QPainter *painter,
                                const QStyleOptionViewItem& option,
                                const QModelIndex& index ) const
{
    Q_ASSERT(index.isValid());
    // TODO: implement clicking on buttons, add configure button.
    QRect rect = option.rect;

    painter->save();

    // Drawing the background
    QStyleOption background = option;
    QApplication::style()->drawPrimitive( QStyle::PE_PanelItemViewItem, &option, painter );

    painter->translate( rect.topLeft() );

    // rect is now represented in item coordinates
    rect.moveTopLeft( QPoint( 0, 0 ) );
    // The point at the top left of the available drawing area.
    QPoint topLeft( 0, 0 );
    // The point at the top right of the available drawing area.
    QPoint topRight( rect.topRight() );

    // Painting the checkbox
    QStyleOptionButton checkBox = checkboxOption( option, index );
    painter->save();
    QApplication::style()->drawControl( QStyle::CE_CheckBox, &checkBox, painter );
    painter->restore();
    topLeft += QPoint( checkBox.rect.width(), 0 );

    // Painting the Name string
    QString name = index.data( Qt::DisplayRole ).toString();
    QRect nameRect( QPoint( 0, 0 ), nameSize( index ) );
    nameRect.setHeight( rect.height() );
    nameRect.moveTopLeft( topLeft );
    QApplication::style()->drawItemText( painter,
                                         nameRect,
                                         Qt::AlignLeft | Qt::AlignVCenter,
                                         option.palette,
                                         false,
                                         name );
    topLeft += QPoint( nameRect.width(), 0 );
    
    // Painting the About Button
    if ( /*index.data( RenderPlugin::AboutDialogAvailable ).toBool()*/ true ) {
        QStyleOptionButton button = buttonOption( option, index, PluginItemDelegate::About );
        button.rect.setHeight( rect.height() );

        button.rect.moveTopRight( topRight );
        QApplication::style()->drawControl( QStyle::CE_PushButton, &button, painter );
        topRight -= QPoint( button.rect.width(), 0 );
    }
    painter->restore();
}

QSize PluginItemDelegate::sizeHint( const QStyleOptionViewItem& option,
                                    const QModelIndex & index ) const
{
    QSize size;

    QStyleOptionViewItem opt = option;
    opt.rect = QRect( 0, 0, 0, 0 );
    QList<QSize> elementSize;
    QStyleOptionButton checkBox = checkboxOption( opt, index );
    elementSize.append( checkBox.rect.size() );
    QStyleOptionButton aboutButton = buttonOption( opt, index, PluginItemDelegate::About );
    elementSize.append( aboutButton.rect.size() );
    elementSize.append( nameSize( index ) );

    foreach( QSize buttonSize, elementSize ) {
        if( buttonSize.height() > size.height() )
            size.setHeight( buttonSize.height() );
        size.setWidth( size.width() + buttonSize.width() );
    }

    return size;
}

bool PluginItemDelegate::editorEvent( QEvent *event,
                                      QAbstractItemModel *model,
                                      const QStyleOptionViewItem &option,
                                      const QModelIndex &index )
{
    Q_ASSERT(event);
    Q_ASSERT(model);

    if ( ( event->type() == QEvent::MouseButtonRelease )
         || ( event->type() == QEvent::MouseButtonDblClick ) )
    {
        QMouseEvent *me = static_cast<QMouseEvent*>(event);

        // Handle checkbox
        QRect checkRect = checkboxOption( option, index ).rect;
        checkRect.translate( option.rect.topLeft() );
        if ( checkRect.contains( me->pos() ) ) {
            // make sure that the item is checkable
            Qt::ItemFlags flags = model->flags(index);
            if ( !( flags & Qt::ItemIsUserCheckable ) || !( option.state & QStyle::State_Enabled )
                || !( flags & Qt::ItemIsEnabled ) )
                return false;

            // make sure that we have a check state
            QVariant checkValue = index.data( Qt::CheckStateRole );
            if ( !checkValue.isValid() )
                return false;

            // eat the double click events inside the check rect
            if ( event->type() == QEvent::MouseButtonDblClick )
                return true;

            Qt::CheckState state = ( static_cast<Qt::CheckState>( checkValue.toInt() ) == Qt::Checked
                                     ? Qt::Unchecked : Qt::Checked );
            return model->setData(index, state, Qt::CheckStateRole);
        }

        // Handle aboutButton
        QRect aboutRect = buttonOption( option, index, PluginItemDelegate::About ).rect;
        aboutRect.moveTopRight( option.rect.topRight() );
        if ( aboutRect.contains( me->pos() ) ) {
            // make sure we have a about button
            if ( /*!index.data( RenderPlugin::AboutDialogAvailable ).toBool()*/ false )
                return false;

            if ( event->type() == QEvent::MouseButtonDblClick )
                return true;

            qDebug() << "About clicked";
            emit aboutPluginClicked( index.data( RenderPlugin::NameId ).toString() );
        }
    }

    return false;
}

QStyleOptionButton PluginItemDelegate::checkboxOption( const QStyleOptionViewItem& option,
                                                       const QModelIndex& index ) const
{
    QSize size = QApplication::style()->sizeFromContents( QStyle::CT_CheckBox, &option, QSize() );
    QStyleOptionButton checkboxOption;
    if ( index.data( Qt::CheckStateRole ).toBool() )
        checkboxOption.state = option.state | QStyle::State_On;
    else
        checkboxOption.state = option.state | QStyle::State_Off;
    checkboxOption.rect.setTopLeft( QPoint( 0, 0 ) );
    checkboxOption.rect.setSize( QSize( size.width(), size.height() ) );

    // Moves the checkbox to the middle of the item.
    checkboxOption.rect.moveTop( ( option.rect.height() - checkboxOption.rect.height() ) / 2 );
    return checkboxOption;
}

QStyleOptionButton PluginItemDelegate::buttonOption( const QStyleOptionViewItem& option,
                                                     const QModelIndex& index,
                                                     PluginItemDelegate::ButtonType type ) const
{
    QStyleOptionButton buttonOption;
    buttonOption.state = option.state;
    buttonOption.state &= ~QStyle::State_HasFocus;

    buttonOption.rect.setTopLeft( QPoint( 0, 0 ) );
    buttonOption.palette = option.palette;
    buttonOption.features = QStyleOptionButton::None;
    if ( type == PluginItemDelegate::About )
        buttonOption.text = tr( "About" );
    else if ( type == PluginItemDelegate::Configure )
        buttonOption.text = tr( "Configure" );
    buttonOption.state = option.state;

    QSize textSize = buttonOption.fontMetrics.size( 0, buttonOption.text ) + QSize( 4, 4 );
    QSize buttonSize = QApplication::style()->sizeFromContents( QStyle::CT_PushButton,
                                                                &buttonOption,
                                                                textSize );
    buttonOption.rect.setSize( buttonSize );
    return buttonOption;
}

QSize PluginItemDelegate::nameSize( const QModelIndex& index ) const {
    QString name = index.data( Qt::DisplayRole ).toString();
    QSize nameSize( QApplication::fontMetrics().size( 0, name ) );
    return nameSize;
}


#include "PluginItemDelegate.moc"
