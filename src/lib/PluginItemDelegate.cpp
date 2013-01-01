//
// This file is part of the Marble Virtual Globe.
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
#include "RenderPluginModel.h"
#include "MarbleDebug.h"

// Qt
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
/* TRANSLATOR Marble::PluginItemDelegate */

const QSize iconSize( 16, 16 );

PluginItemDelegate::PluginItemDelegate( QAbstractItemView *view, QObject * parent )
    : QAbstractItemDelegate( parent )
{
    // Enable mouse tracking of itemview makes it possible to find when the mouse if moved
    // without pressed buttons.
    view->setMouseTracking( true );
}

PluginItemDelegate::~PluginItemDelegate()
{
}

void PluginItemDelegate::paint( QPainter *painter,
                                const QStyleOptionViewItem& option,
                                const QModelIndex& index ) const
{
    Q_ASSERT( index.isValid() );
    QRect rect = option.rect;
    QStyle *style = QApplication::style();

    painter->save();

    // Drawing the background
    QStyleOption background = option;
    style->drawPrimitive( QStyle::PE_PanelItemViewItem, &option, painter );

    painter->translate( rect.topLeft() );

    // rect is now represented in item coordinates
    rect.moveTopLeft( QPoint( 0, 0 ) );
    // The point at the top left of the available drawing area.
    QPoint topLeft( 0, 0 );
    // The point at the top right of the available drawing area.
    QPoint topRight( rect.topRight() );

    QRect nameRect = rect;
    
    // Painting the checkbox
    QStyleOptionButton checkBox = checkboxOption( option, index, topLeft.x(), Qt::AlignLeft );
    painter->save();
    style->drawControl( QStyle::CE_CheckBox, &checkBox, painter );
    painter->restore();

    nameRect.setLeft( checkBox.rect.right() + 1 );
    
    // Painting the About Button
    QStyleOptionButton button = buttonOption( option, index, PluginItemDelegate::About,
                                              topRight.x(), Qt::AlignRight );
    style->drawControl( QStyle::CE_PushButton, &button, painter );
    topRight -= QPoint( button.rect.width(), 0 );

    // Painting the Configure Button
    if ( index.data( RenderPluginModel::ConfigurationDialogAvailable ).toBool() ) {
        QStyleOptionButton button = buttonOption( option, index, PluginItemDelegate::Configure,
                                                  topRight.x(), Qt::AlignRight );
        style->drawControl( QStyle::CE_PushButton, &button, painter );
        topRight -= QPoint( button.rect.width(), 0 );
        
        nameRect.setRight( button.rect.left() -1 );
    }

    // Painting the Icon
    const QIcon icon = index.data( Qt::DecorationRole ).value<QIcon>();
    const QPixmap iconPixmap = icon.pixmap(16, 16);

    nameRect.moveBottom( nameRect.bottom()+5 );
    style->drawItemPixmap( painter,
                           nameRect,
                           Qt::AlignLeft,
                           iconPixmap );

    nameRect.setLeft( nameRect.left() + 16 + 5 );
    nameRect.moveBottom( nameRect.bottom()-5 );

    // Painting the Name string
    QString name = index.data( Qt::DisplayRole ).toString();
    
    style->drawItemText( painter,
                         nameRect,
                         Qt::AlignLeft | Qt::AlignVCenter,
                         option.palette,
                         true,
                         name );

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
    QStyleOptionButton configButton = buttonOption( opt, index, PluginItemDelegate::Configure );
    elementSize.append( configButton.rect.size() );
    elementSize.append( nameSize( index ) );

    foreach( const QSize& buttonSize, elementSize ) {
        if( buttonSize.height() > size.height() )
            size.setHeight( buttonSize.height() );
        size.setWidth( size.width() + buttonSize.width() );
    }

    return size;
}

void PluginItemDelegate::setAboutIcon( const QIcon& icon )
{
    m_aboutIcon = icon;
}

void PluginItemDelegate::setConfigIcon( const QIcon& icon )
{
    m_configIcon = icon;
}

bool PluginItemDelegate::editorEvent( QEvent *event,
                                      QAbstractItemModel *model,
                                      const QStyleOptionViewItem &option,
                                      const QModelIndex &index )
{
    Q_ASSERT(event);
    Q_ASSERT(model);

    if ( ( event->type() == QEvent::MouseButtonRelease )
         || ( event->type() == QEvent::MouseButtonDblClick )
         || ( event->type() == QEvent::MouseButtonPress )
         || ( event->type() == QEvent::MouseMove ) )
    {
        QMouseEvent *me = static_cast<QMouseEvent*>(event);
        QPoint mousePosition = me->pos() - option.rect.topLeft();

        if ( ( event->type() == QEvent::MouseMove )
             && !( me->buttons() & Qt::LeftButton ) )
        {
            // If the mouse moves around and no left button is pressed, no pushbutton is pressed
            // and no other event will be successful.
            m_aboutPressedIndex = QModelIndex();
            m_configPressedIndex = QModelIndex();
            return true;
        }

        // Handle checkbox
        QRect checkRect = checkboxOption( option, index, 0, Qt::AlignLeft ).rect;
        if ( checkRect.contains( mousePosition )
             && ( ( event->type() == QEvent::MouseButtonDblClick )
                   || ( event->type() == QEvent::MouseButtonRelease ) ) )
        {
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

        if ( ( event->type() == QEvent::MouseMove )
             && !( me->buttons() & Qt::LeftButton ) )
        {
            m_aboutPressedIndex = QModelIndex();
            m_configPressedIndex = QModelIndex();
            return true;
        }

        QPoint topRight = option.rect.topRight();

        // Handle aboutButton
        {
            QRect aboutRect = buttonOption( option,
                                            index,
                                            PluginItemDelegate::About,
                                            topRight.x(),
                                            Qt::AlignRight ).rect;
            if ( aboutRect.contains( mousePosition ) ) {
                if ( event->type() == QEvent::MouseButtonDblClick )
                    return true;
                if ( event->type() == QEvent::MouseButtonPress ) {
                    m_aboutPressedIndex = index;
                    m_configPressedIndex = QModelIndex();
                    return true;
                }
                if ( event->type() == QEvent::MouseButtonRelease ) {
                    m_aboutPressedIndex = QModelIndex();
                    m_configPressedIndex = QModelIndex();
                    emit aboutPluginClicked( index );
                    return true;
                }
                if ( event->type() == QEvent::MouseMove ) {
                    if ( me->buttons() & Qt::LeftButton ) {
                        m_aboutPressedIndex = index;
                        m_configPressedIndex = QModelIndex();
                        return true;
                    }
                    else {
                        m_aboutPressedIndex = QModelIndex();
                        m_configPressedIndex = QModelIndex();
                        return true;
                    }
                }
            }
            else {
                // If the mouse is on the item and the mouse isn't above the button.
                // no about button is pressed.
                m_aboutPressedIndex = QModelIndex();
            }
            topRight -= QPoint( aboutRect.width(), 0 );
        }

        // Handle configButton
        // make sure we have config button
        if ( index.data( RenderPluginModel::ConfigurationDialogAvailable ).toBool() ) {
            QRect configRect = buttonOption( option,
                                             index,
                                             PluginItemDelegate::Configure,
                                             topRight.x(),
                                             Qt::AlignRight ).rect;
            if( configRect.contains( mousePosition ) ) {
                if ( event->type() == QEvent::MouseButtonDblClick )
                    return true;

                if ( event->type() == QEvent::MouseButtonPress ) {
                    m_aboutPressedIndex = QModelIndex();
                    m_configPressedIndex = index;
                    return true;
                }
                if ( event->type() == QEvent::MouseButtonRelease ) {
                    m_aboutPressedIndex = QModelIndex();
                    m_configPressedIndex = QModelIndex();
                    emit configPluginClicked( index );
                    return true;
                }
                if ( event->type() == QEvent::MouseMove ) {
                    if ( me->buttons() & Qt::LeftButton ) {
                        m_aboutPressedIndex = QModelIndex();
                        m_configPressedIndex = index;
                        return true;
                    }
                    else {
                        m_aboutPressedIndex = QModelIndex();
                        m_configPressedIndex = QModelIndex();
                        return true;
                    }
                }
            }
            else {
                // If the mouse is on the item and the mouse isn't above the button.
                // no config button is pressed.
                m_configPressedIndex = QModelIndex();
            }

            topRight -= QPoint( configRect.width(), 0 );
        }
        else {
            // If we don't have an config dialog shown and the mouse is over this item,
            // no config button is pressed.
            m_configPressedIndex = QModelIndex();
        }
    }

    return false;
}

QStyleOptionButton PluginItemDelegate::checkboxOption( const QStyleOptionViewItem& option,
                                                       const QModelIndex& index,
                                                       int position,
                                                       Qt::AlignmentFlag alignment ) const
{
    QStyleOptionButton checkboxOption;
    if ( index.data( Qt::CheckStateRole ).toBool() )
        checkboxOption.state = option.state | QStyle::State_On;
    else
        checkboxOption.state = option.state | QStyle::State_Off;
    QSize size = QApplication::style()->sizeFromContents( QStyle::CT_CheckBox, &option, QSize() );
    if ( size.isEmpty() ) {
        // A checkbox has definitely a size != 0
        checkboxOption.rect.setSize( QSize( 22, 22 ) );
    }
    else {
        checkboxOption.rect.setSize( QSize( size.width(), size.height() ) );
    }
    checkboxOption.rect = alignRect( checkboxOption.rect, option.rect, position, alignment );
    return checkboxOption;
}

QStyleOptionButton PluginItemDelegate::buttonOption( const QStyleOptionViewItem& option,
                                                     const QModelIndex& index,
                                                     PluginItemDelegate::ButtonType type,
                                                     int position, 
                                                     Qt::AlignmentFlag alignment ) const
{
    QStyleOptionButton buttonOption;
    buttonOption.state = option.state;
    buttonOption.state &= ~QStyle::State_HasFocus;

    buttonOption.rect.setTopLeft( QPoint( 0, 0 ) );
    buttonOption.palette = option.palette;
    buttonOption.features = QStyleOptionButton::None;

    QSize contentSize;
    if ( type == PluginItemDelegate::About ) {
        if ( m_aboutIcon.isNull() ) {
            buttonOption.text = tr( "About" );
            contentSize = buttonOption.fontMetrics.size( 0, buttonOption.text ) + QSize( 4, 4 );
        }
        else {
            buttonOption.icon = m_aboutIcon;
            buttonOption.iconSize = iconSize;
            contentSize = iconSize;
        }

        if ( m_aboutPressedIndex == index ) {
            buttonOption.state |= QStyle::State_Sunken;
        }
    }
    else if ( type == PluginItemDelegate::Configure ) {
        if ( m_configIcon.isNull() ) {
            buttonOption.text = tr( "Configure" );
            contentSize = buttonOption.fontMetrics.size( 0, buttonOption.text ) + QSize( 4, 4 );
        }
        else {
            buttonOption.icon = m_configIcon;
            buttonOption.iconSize = iconSize;
            contentSize = iconSize;
        }
        if ( m_configPressedIndex == index ) {
            buttonOption.state |= QStyle::State_Sunken;
        }
    }

    QSize buttonSize = QApplication::style()->sizeFromContents( QStyle::CT_PushButton,
                                                                &buttonOption,
                                                                contentSize );
    buttonOption.rect.setSize( buttonSize );
    buttonOption.rect = alignRect( buttonOption.rect, option.rect, position, alignment );
    return buttonOption;
}

QSize PluginItemDelegate::nameSize( const QModelIndex& index ) const
{
    QString name = index.data( Qt::DisplayRole ).toString();
    // FIXME: QApplication::fontMetrics() doesn't work for non-application fonts
    QSize nameSize( QApplication::fontMetrics().size( 0, name ) );
    return nameSize;
}

QRect PluginItemDelegate::alignRect( QRect object,
                                     QRect frame,
                                     int position,
                                     Qt::AlignmentFlag alignment ) const
{
    QRect rect = object;
    
    rect.setTopLeft( QPoint( 0, 0 ) );
    // Moves the object to the middle of the item.
    if ( rect.height() < frame.height() ) {
        rect.moveTop( ( frame.height() - rect.height() ) / 2 );
    }

    if ( alignment & Qt::AlignLeft ) {
        rect.moveLeft( position );
    }
    else if ( alignment & Qt::AlignRight ) {
        rect.moveRight( position );
    }
    
    return rect;
}


#include "PluginItemDelegate.moc"
