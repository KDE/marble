//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009 Bastian Holst <bastianholst@gmx.de>
//

#ifndef PLUGINITEMDELEGATE_H
#define PLUGINITEMDELEGATE_H

#include <QtGui/QStyledItemDelegate>

class QPainter;
class QStyleOptionViewItem;
class QModelIndex;
class QAbstractViewItem;

namespace Marble {

class PluginItemDelegate : public QStyledItemDelegate
{
    Q_OBJECT

 public:
    PluginItemDelegate( QAbstractItemView *itemView, QObject * parent = 0 );
    ~PluginItemDelegate();
    
    void paint( QPainter *painter,
                const QStyleOptionViewItem& option,
                const QModelIndex& index ) const;
    QSize sizeHint( const QStyleOptionViewItem& option, const QModelIndex & index ) const;
    
 Q_SIGNALS:
    /**
     * This signal is emitted if the user clicks on a "about"-button of an item in the view
     * passed to the constructor.
     */
    void aboutPluginClicked( QString nameId );

    /**
     * This signal is emitted if the user clicks on a "configure"-button of an item in the view
     * passed to the constructor.
     */
    void configPluginClicked( QString nameId );

 private Q_SLOTS:
    void handleClickEvent( const QModelIndex& );

 private:
    const QAbstractItemView *m_itemView;
};

}

#endif // PLUGINITEMDELEGATE_H
