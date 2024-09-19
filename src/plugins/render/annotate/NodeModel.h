// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2015 Mikhail Ivchenko <ematirov@gmail.com>
//

#ifndef NODEMODEL_H
#define NODEMODEL_H

#include "GeoDataCoordinates.h"
#include <QAbstractListModel>

namespace Marble
{

class NodeModel : public QAbstractListModel
{
    Q_OBJECT
public:
    explicit NodeModel(QObject *parent = nullptr);
    int rowCount(const QModelIndex &parent) const override;
    int columnCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    void clear();
    Qt::ItemFlags flags(const QModelIndex &index) const override;
public Q_SLOTS:
    int addNode(const GeoDataCoordinates &node);

private:
    QVector<GeoDataCoordinates> m_nodes;
};

}

#endif
