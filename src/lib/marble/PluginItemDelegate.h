//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009 Bastian Holst <bastianholst@gmx.de>
//

#ifndef MARBLE_PLUGINITEMDELEGATE_H
#define MARBLE_PLUGINITEMDELEGATE_H

#include <QAbstractItemDelegate>

#include <QModelIndex>
#include <QIcon>

class QPainter;
class QRect;
class QStyleOptionButton;
class QStyleOptionViewItem;

namespace Marble
{

class PluginItemDelegate : public QAbstractItemDelegate
{
    Q_OBJECT

 public:
    explicit PluginItemDelegate( QAbstractItemView *view, QObject * parent = 0 );
    ~PluginItemDelegate();
    
    void paint( QPainter *painter,
                const QStyleOptionViewItem& option,
                const QModelIndex& index ) const;
    QSize sizeHint( const QStyleOptionViewItem& option, const QModelIndex & index ) const;

    void setAboutIcon( const QIcon& icon );
    void setConfigIcon( const QIcon& icon );
    
 Q_SIGNALS:
    /**
     * This signal is emitted if the user clicks on a "about"-button of an item in the view
     * passed to the constructor.
     */
    void aboutPluginClicked( const QModelIndex &index );

    /**
     * This signal is emitted if the user clicks on a "configure"-button of an item in the view
     * passed to the constructor.
     */
    void configPluginClicked( const QModelIndex &index );

 protected:
    bool editorEvent( QEvent *event,
                      QAbstractItemModel *model,
                      const QStyleOptionViewItem &option,
                      const QModelIndex &index );

 private:
    enum ButtonType {
        About,
        Configure
    };

    static QStyleOptionButton checkboxOption( const QStyleOptionViewItem& option,
                                              const QModelIndex& index,
                                              int position = 0,
                                              Qt::AlignmentFlag alignment = Qt::AlignLeft );
    QStyleOptionButton buttonOption(   const QStyleOptionViewItem& option,
                                       const QModelIndex& index,
                                       PluginItemDelegate::ButtonType type,
                                       int position = 0,
                                       Qt::AlignmentFlag alignment = Qt::AlignLeft ) const;
    static QSize nameSize( const QModelIndex& index );
    
    static QRect alignRect( const QRect& object, const QRect& frame, int position, Qt::AlignmentFlag alignment );

    QModelIndex m_configPressedIndex;
    QModelIndex m_aboutPressedIndex;

    QIcon m_aboutIcon;
    QIcon m_configIcon;
};

}

#endif
