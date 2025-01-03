// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2015 Stanciu Marius-Valeriu <stanciumarius94@gmail.com>
//

#ifndef NODEITEMDELEGATE_H
#define NODEITEMDELEGATE_H

// Qt
#include <QItemDelegate>
#include <QTreeView>

// Marble
#include "EditPolygonDialog.h"

namespace Marble
{

class GeoDataPlacemark;

/**
 * @brief The NodeItemDelegate class handles the NodeModel view for both the EditPolygonDialog
 * and EditPolylineDialog. It manages editing and updating the NodeModel and triggers drawing
 * updates when changes are made.
 */
class NodeItemDelegate : public QItemDelegate
{
    Q_OBJECT

public:
    NodeItemDelegate(GeoDataPlacemark *placemark, QTreeView *view);
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    void setEditorData(QWidget *editor, const QModelIndex &index) const override;
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const override;
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;

private:
    GeoDataPlacemark *const m_placemark;
    mutable QModelIndex m_indexBeingEdited;
    QTreeView *const m_view;

private Q_SLOTS:
    void previewNodeMove(qreal value);
    void unsetCurrentEditor(QWidget *widget);

Q_SIGNALS:
    void modelChanged(GeoDataPlacemark *placemark) const;
    void geometryChanged() const;
};

}
#endif
