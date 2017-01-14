//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2017      Sergey Popov <sergobot@protonmail.com>
//

#ifndef MARBLE_DECLARATIVE_ROUTERELATIONMODEL_H
#define MARBLE_DECLARATIVE_ROUTERELATIONMODEL_H

#include <QAbstractListModel>
#include <QVector>

#include "GeoDataRelation.h"

namespace Marble {

class RouteRelationModel : public QAbstractListModel
{
    Q_OBJECT
public:
    enum RelationRoles {
        RelationName = Qt::UserRole + 1
    };

    RouteRelationModel(QObject* parent = 0);
    
    void setRelations(const QVector<const Marble::GeoDataRelation*> &relations);
    
    int rowCount(const QModelIndex & parent = QModelIndex()) const;
    
    QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;

protected:
    QHash<int, QByteArray> roleNames() const;
    
private:
    QVector<const Marble::GeoDataRelation*> m_relations;
};

}

#endif // ROUTERELATIONMODEL
