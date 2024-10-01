// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2017 Sergey Popov <sergobot@protonmail.com>
//

#ifndef MARBLE_DECLARATIVE_ROUTERELATIONMODEL_H
#define MARBLE_DECLARATIVE_ROUTERELATIONMODEL_H

#include <QAbstractListModel>
#include <QSet>

#include "GeoDataRelation.h"

namespace Marble
{

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

    explicit RouteRelationModel(QObject *parent = nullptr);

    void setRelations(const QSet<const GeoDataRelation *> &relations);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

protected:
    QHash<int, QByteArray> roleNames() const override;

private:
    static QString svgFile(const QString &path);

    QList<const Marble::GeoDataRelation *> m_relations;
    QMap<QString, QString> m_networks;
};

}

#endif // ROUTERELATIONMODEL
