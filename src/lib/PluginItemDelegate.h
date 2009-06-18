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
class QModelIndex;
class QRect;
class QStyleOptionButton;
class QStyleOptionViewItem;

namespace Marble {

class PluginItemDelegate : public QAbstractItemDelegate
{
    Q_OBJECT

 public:
    PluginItemDelegate( QObject * parent = 0 );
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

 protected:
    bool editorEvent( QEvent *event,
                      QAbstractItemModel *model,
                      const QStyleOptionViewItem &option,
                      const QModelIndex &index );

 private:
    QStyleOptionButton checkboxOption( const QStyleOptionViewItem& option,
                                       const QModelIndex& index ) const;
    QStyleOptionButton buttonOption(   const QStyleOptionViewItem& option,
                                       const QModelIndex& index ) const;
    QSize nameSize( const QModelIndex& index ) const;
};

}

#endif // PLUGINITEMDELEGATE_H
