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
#include <QSet>

#include "GeoDataRelation.h"

namespace Marble {

class RouteRelationModel : public QAbstractListModel
{
    Q_OBJECT

public:
    enum RouteRelationRoles {
        IconSource = Qt::UserRole + 1,
        Description,
        Network,
        RouteColor,
        TextColor,
        RouteFrom,
        RouteTo,
        RouteRef,
        RouteVia,
        OsmId,
        RouteVisible
    };

    RouteRelationModel(QObject* parent = 0);
    
    void setRelations(const QSet<const GeoDataRelation *> &relations);
    
    int rowCount(const QModelIndex & parent = QModelIndex()) const override;
    
    QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const override;

protected:
    QHash<int, QByteArray> roleNames() const override;

private:
    QString svgFile(const QString &path) const;

    QVector<const Marble::GeoDataRelation*> m_relations;
    QMap<QString, QString> m_networks;
};

}

#endif // ROUTERELATIONMODEL
