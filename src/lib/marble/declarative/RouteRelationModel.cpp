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

#include "MarbleDirs.h"

namespace Marble
{

RouteRelationModel::RouteRelationModel(QObject *parent) :
    QAbstractListModel(parent)
{
    // nothing to do
}

void RouteRelationModel::setRelations(const QVector<const GeoDataRelation*> &relations)
{
    if (!m_relations.isEmpty()) {
        beginRemoveRows(QModelIndex(), 0, m_relations.count() - 1);
        m_relations.clear();
        endRemoveRows();
    }

    if (!relations.isEmpty()) {
        beginInsertRows(QModelIndex(), 0, relations.count() - 1);
        m_relations.reserve(relations.size());
        std::copy_if(relations.begin(), relations.end(), std::back_inserter(m_relations),
        [](const GeoDataRelation * relation) {
            return relation->relationType() >= GeoDataRelation::RouteRoad && relation->relationType() <= GeoDataRelation::RouteSled;
        });
        std::sort(m_relations.begin(), m_relations.end(),
        [](const GeoDataRelation * a, const GeoDataRelation * b) {
            return a->relationType() < b->relationType();
        });
        endInsertRows();
    }
}

int RouteRelationModel::rowCount(const QModelIndex & parent) const
{
    return parent.isValid() ? 0 : m_relations.count();
}

QVariant RouteRelationModel::data(const QModelIndex & index, int role) const
{
    if (!index.isValid() || index.row() < 0 || index.row() >= m_relations.count()) {
        return QVariant();
    }

    if (role == Qt::DisplayRole) {
        return m_relations.at(index.row())->name();
    } else if (role == IconSource) {
        switch (m_relations.at(index.row())->relationType()) {
        case GeoDataRelation::RouteRoad:         return MarbleDirs::path(QStringLiteral("svg/material/maps/ic_directions_bike_48px.svg"));
        case GeoDataRelation::RouteDetour:       return MarbleDirs::path(QStringLiteral("svg/material/maps/ic_directions_car_48px.svg"));
        case GeoDataRelation::RouteFerry:        return MarbleDirs::path(QStringLiteral("svg/material/maps/ic_directions_boat_48px.svg"));
        case GeoDataRelation::RouteTrain:        return MarbleDirs::path(QStringLiteral("svg/material/maps/ic_directions_railway_48px.svg"));
        case GeoDataRelation::RouteSubway:       return MarbleDirs::path(QStringLiteral("svg/material/maps/ic_directions_subway_48px.svg"));
        case GeoDataRelation::RouteTram:         return MarbleDirs::path(QStringLiteral("svg/material/maps/ic_tram_48px.svg"));
        case GeoDataRelation::RouteBus:          return MarbleDirs::path(QStringLiteral("svg/material/maps/ic_directions_bus_48px.svg"));
        case GeoDataRelation::RouteTrolleyBus:   return MarbleDirs::path(QStringLiteral("svg/material/maps/ic_directions_bus_48px.svg"));
        case GeoDataRelation::RouteBicycle:      return MarbleDirs::path(QStringLiteral("svg/material/maps/ic_directions_bike_48px.svg"));
        case GeoDataRelation::RouteMountainbike: return MarbleDirs::path(QStringLiteral("svg/material/maps/ic_directions_bike_48px.svg"));
        case GeoDataRelation::RouteFoot:         return MarbleDirs::path(QStringLiteral("svg/material/maps/ic_directions_walk_48px.svg"));
        case GeoDataRelation::RouteHiking:       return MarbleDirs::path(QStringLiteral("svg/thenounproject/204712-hiker.svg"));
        case GeoDataRelation::RouteHorse:        return MarbleDirs::path(QStringLiteral("svg/thenounproject/78374-horse-riding.svg"));
        case GeoDataRelation::RouteInlineSkates: return MarbleDirs::path(QStringLiteral("svg/thenounproject/101965-inline-skater.svg"));
        case GeoDataRelation::RouteSkiDownhill:  return MarbleDirs::path(QStringLiteral("svg/thenounproject/2412-skiing-downhill.svg"));
        case GeoDataRelation::RouteSkiNordic:    return MarbleDirs::path(QStringLiteral("svg/thenounproject/30231-skiing-cross-country.svg"));
        case GeoDataRelation::RouteSkitour:      return MarbleDirs::path(QStringLiteral("svg/thenounproject/29366-skitour.svg"));
        case GeoDataRelation::RouteSled:         return MarbleDirs::path(QStringLiteral("svg/thenounproject/365217-sled.svg"));
        case GeoDataRelation::UnknownType:       return QVariant(QString());
        }
    }

    return QVariant();
}

QHash<int, QByteArray> RouteRelationModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[Qt::DisplayRole] = "display";
    roles[IconSource] = "iconSource";
    return roles;
}

}
