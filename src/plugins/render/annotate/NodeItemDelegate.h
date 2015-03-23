//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2015      Stanciu Marius-Valeriu  <stanciumarius94@gmail.com>
//

#ifndef NODEITEMDELEGATE_H
#define NODEITEMDELEGATE_H

// Qt
#include <QItemDelegate>
#include <QTreeView>

// Marble
#include "GeoDataPlacemark.h"
#include "EditPolygonDialog.h"

namespace Marble
{

class NodeItemDelegate : public QItemDelegate
{

Q_OBJECT

public:
    NodeItemDelegate( GeoDataPlacemark* placemark, EditPolygonDialog* dialog, QTreeView* view );
    QSize sizeHint( const QStyleOptionViewItem &option, const QModelIndex &index ) const;
    QWidget* createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    void setEditorData(QWidget *editor, const QModelIndex &index) const;
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;

private:
    GeoDataPlacemark* m_placemark;
    EditPolygonDialog* m_dialog;
    mutable QModelIndex m_indexBeingEdited;
    QTreeView* m_view;

private slots:
    void previewNodeMove( qreal value);
    void unsetCurrentEditor( QWidget* widget );
};

}
#endif
