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
#include <QtCore/QVariant>
#include <QtGui/QAbstractItemView>
#include <QtGui/QStandardItemModel>
#include <QtGui/QApplication>
#include <QtGui/QPainter>
#include <QtGui/QStandardItem>

using namespace Marble;

PluginItemDelegate::PluginItemDelegate( QAbstractItemView *itemView, QObject * parent )
    : QStyledItemDelegate( parent ),
      m_itemView( itemView )
{
    connect( itemView, SIGNAL( clicked( QModelIndex ) ),
                       SLOT( handleClickEvent( const QModelIndex& ) ) );
}

PluginItemDelegate::~PluginItemDelegate() {
}

void PluginItemDelegate::paint( QPainter *painter,
                                const QStyleOptionViewItem& option,
                                const QModelIndex& index ) const
{
    Q_ASSERT(index.isValid());
    // TODO: Do background rendering, implement clicking on buttons, add configure button.
    QRect rect = option.rect;

    painter->save();

    painter->translate( rect.topLeft() );

    // Painting the checkbox
    QStyleOptionButton checkboxOption;
    if ( index.data( Qt::CheckStateRole ).toBool() )
        checkboxOption.state = option.state | QStyle::State_On;
    else
        checkboxOption.state = option.state | QStyle::State_Off;
    checkboxOption.rect.setTopLeft( QPoint( 0, 0 ) );
    checkboxOption.rect.setSize( QSize( rect.height(), rect.height() ) );
    painter->save();
    QApplication::style()->drawControl( QStyle::CE_CheckBox, &checkboxOption, painter );
    painter->restore();
    painter->translate( checkboxOption.rect.width(), 0 );


    // Painting the Name string
    QString name = index.data( Qt::DisplayRole ).toString();
    QRect nameRect( QPoint( 0, 0 ), QApplication::fontMetrics().size( 0, name ) );
    nameRect.setHeight( rect.height() );
    QApplication::style()->drawItemText( painter,
                                         nameRect,
                                         Qt::AlignLeft | Qt::AlignVCenter,
                                         option.palette,
                                         false,
                                         name );
    painter->translate( nameRect.width(), 0 );
    
    // Painting the About Button
    if ( /*index.data( RenderPlugin::AboutDialogAvailable ).toBool()*/ true ) {
        QStyleOptionButton buttonOption;
        buttonOption.state = option.state;
        buttonOption.state &= ~QStyle::State_HasFocus;

        buttonOption.rect.setTopLeft( QPoint( 0, 0 ) );
        buttonOption.palette = option.palette;
        buttonOption.features = QStyleOptionButton::None;
        buttonOption.text = tr( "About" );
        buttonOption.state = option.state;

        QSize aboutSize = buttonOption.fontMetrics.size( 0, buttonOption.text ) + QSize( 4, 4 );
        QSize aboutButtonSize = QApplication::style()->sizeFromContents( QStyle::CT_PushButton,
                                                                         &buttonOption,
                                                                         aboutSize );
        buttonOption.rect.setWidth( aboutButtonSize.width() );
        buttonOption.rect.setHeight( rect.height() );

        QApplication::style()->drawControl( QStyle::CE_PushButton, &buttonOption, painter );
        painter->translate( aboutButtonSize.width(), 0 );
    }
    painter->restore();
}

QSize PluginItemDelegate::sizeHint( const QStyleOptionViewItem& option,
                                    const QModelIndex & index ) const
{
    QSize sz = QStyledItemDelegate::sizeHint(option, index) + QSize( 4, 4 );
    return sz;
}

void PluginItemDelegate::handleClickEvent( const QModelIndex& ) {
    // TODO: Click handling.
}

#include "PluginItemDelegate.moc"
