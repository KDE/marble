// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2004-2007 Torsten Rahn <tackat@kde.org>
// SPDX-FileCopyrightText: 2007 Inge Wallin <ingwa@kde.org>
// SPDX-FileCopyrightText: 2007 Thomas Zander <zander@kde.org>
// SPDX-FileCopyrightText: 2010 Bastian Holst <bastianholst@gmx.de>
// SPDX-FileCopyrightText: 2011-2013 Bernhard Beschow <bbeschow@cs.tu-berlin.de>
// SPDX-FileCopyrightText: 2012 Illya Kovalevskyy <illya.kovalevskyy@gmail.com>
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
    void paint( QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index ) const override;
    QSize sizeHint( const QStyleOptionViewItem &option, const QModelIndex &index ) const override;

private:
    static QString text( const QModelIndex &index );
    QListView* m_view;
    QIcon m_bookmarkIcon;
};

} //Namespace: Marble

#endif // MAPVIEWITEMDELEGATE_H
