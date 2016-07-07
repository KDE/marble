//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2004-2007 Torsten Rahn  <tackat@kde.org>
// Copyright 2007      Inge Wallin   <ingwa@kde.org>
// Copyright 2007      Thomas Zander <zander@kde.org>
// Copyright 2010      Bastian Holst <bastianholst@gmx.de>
// Coprright 2011-2013 Bernhard Beschow <bbeschow@cs.tu-berlin.de>
// Copyright 2012      Illya Kovalevskyy  <illya.kovalevskyy@gmail.com>
//

#ifndef MAPVIEWITEMDELEGATE_H
#define MAPVIEWITEMDELEGATE_H

#include <QStyledItemDelegate>

class QListView;

namespace Marble
{

/**
 * @brief The MapViewItemDelegate class is a delegate class for both the MapViewWidget's listView
 * and MapChangeEditDialog's listView.
 */
class MapViewItemDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    explicit MapViewItemDelegate( QListView* view );
    void paint( QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index ) const;
    QSize sizeHint( const QStyleOptionViewItem &option, const QModelIndex &index ) const;

private:
    static QString text( const QModelIndex &index );
    QListView* m_view;
    QIcon m_bookmarkIcon;
};

} //Namespace: Marble

#endif // MAPVIEWITEMDELEGATE_H
