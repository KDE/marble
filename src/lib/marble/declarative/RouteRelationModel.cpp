//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2017      Sergey Popov <sergobot@protonmail.com>
//

#include "RouteRelationModel.h"

namespace Marble {

RouteRelationModel::RouteRelationModel(QObject *parent) :
    QAbstractListModel(parent)
{
    // nothing to do
}

void RouteRelationModel::setRelations(const QVector<const Marble::GeoDataRelation*> &relations)
{
    beginRemoveRows(QModelIndex(), 0, m_relations.count() - 1);
    m_relations.clear();
    endRemoveRows();
    
    beginInsertRows(QModelIndex(), 0, relations.count() - 1);
    m_relations = relations;
    endInsertRows();
}

int RouteRelationModel::rowCount(const QModelIndex & parent) const
{
    Q_UNUSED(parent);
    return m_relations.count();
}

QVariant RouteRelationModel::data(const QModelIndex & index, int role) const
{
    if (!index.isValid() || index.row() < 0 || index.row() >= m_relations.count())
        return QVariant();
    
    return m_relations.at(index.row())->name();
}

QHash<int, QByteArray> RouteRelationModel::roleNames() const {
    QHash<int, QByteArray> roles;
    roles[RelationName] = "name";
    return roles;
}

}
