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
#include "osm/OsmPlacemarkData.h"

namespace Marble
{

RouteRelationModel::RouteRelationModel(QObject *parent) :
    QAbstractListModel(parent)
{
    m_networks[QStringLiteral("iwn")] = tr("International walking route");
    m_networks[QStringLiteral("nwn")] = tr("National walking route");
    m_networks[QStringLiteral("rwn")] = tr("Regional walking route");
    m_networks[QStringLiteral("lwn")] = tr("Local walking route");
    m_networks[QStringLiteral("icn")] = tr("International cycling route");
    m_networks[QStringLiteral("ncn")] = tr("National cycling route");
    m_networks[QStringLiteral("rcn")] = tr("Regional cycling route");
    m_networks[QStringLiteral("lcn")] = tr("Local cycling route");
    m_networks[QStringLiteral("US:TX:FM")] = tr("Farm to Market Road", "State or county road in Texas, USA");
    m_networks[QStringLiteral("regional")] = tr("Regional route");
    m_networks[QStringLiteral("national")] = tr("National route");
    m_networks[QStringLiteral("municipal")] = tr("Municipal route");
    m_networks[QStringLiteral("territorial")] = tr("Territorial route");
    m_networks[QStringLiteral("local")] = tr("Local route");
    m_networks[QStringLiteral("prefectural")] = tr("Prefectural route");
    m_networks[QStringLiteral("US")] = tr("United States route");
}

void RouteRelationModel::setRelations(const QSet<const GeoDataRelation*> &relations)
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
            if (a->relationType() == b->relationType()) {
                auto const refA = a->osmData().tagValue(QStringLiteral("ref"));
                auto const refB = b->osmData().tagValue(QStringLiteral("ref"));
                if (refA == refB) {
                    return a->name() < b->name();
                }
                return refA < refB;
            }
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
        case GeoDataRelation::RouteRoad:         return svgFile("material/maps/ic_directions_car_48px.svg");
        case GeoDataRelation::RouteDetour:       return svgFile("material/maps/ic_directions_car_48px.svg");
        case GeoDataRelation::RouteFerry:        return svgFile("material/maps/ic_directions_boat_48px.svg");
        case GeoDataRelation::RouteTrain:        return svgFile("material/maps/ic_directions_railway_48px.svg");
        case GeoDataRelation::RouteSubway:       return svgFile("material/maps/ic_directions_subway_48px.svg");
        case GeoDataRelation::RouteTram:         return svgFile("material/maps/ic_tram_48px.svg");
        case GeoDataRelation::RouteBus:          return svgFile("material/maps/ic_directions_bus_48px.svg");
        case GeoDataRelation::RouteTrolleyBus:   return svgFile("material/maps/ic_directions_bus_48px.svg");
        case GeoDataRelation::RouteBicycle:      return svgFile("material/maps/ic_directions_bike_48px.svg");
        case GeoDataRelation::RouteMountainbike: return svgFile("material/maps/ic_directions_bike_48px.svg");
        case GeoDataRelation::RouteFoot:         return svgFile("material/maps/ic_directions_walk_48px.svg");
        case GeoDataRelation::RouteHiking:       return svgFile("thenounproject/204712-hiker.svg");
        case GeoDataRelation::RouteHorse:        return svgFile("thenounproject/78374-horse-riding.svg");
        case GeoDataRelation::RouteInlineSkates: return svgFile("thenounproject/101965-inline-skater.svg");
        case GeoDataRelation::RouteSkiDownhill:  return svgFile("thenounproject/2412-skiing-downhill.svg");
        case GeoDataRelation::RouteSkiNordic:    return svgFile("thenounproject/30231-skiing-cross-country.svg");
        case GeoDataRelation::RouteSkitour:      return svgFile("thenounproject/29366-skitour.svg");
        case GeoDataRelation::RouteSled:         return svgFile("thenounproject/365217-sled.svg");
        case GeoDataRelation::UnknownType:       return QVariant(QString());
        }
    } else if (role == Description) {
        return m_relations.at(index.row())->osmData().tagValue(QStringLiteral("description"));
    } else if (role == Network) {
        auto const network = m_relations.at(index.row())->osmData().tagValue(QStringLiteral("network"));
        auto iter = m_networks.find(network);
        if (iter != m_networks.end()) {
            return *iter;
        }
        auto const fields = network.split(':', QString::SkipEmptyParts);
        for (auto const &field: fields) {
            auto iter = m_networks.find(field);
            if (iter != m_networks.end()) {
                return *iter;
            }
        }
        return network;
    } else if (role == RouteColor) {
        auto const color = m_relations.at(index.row())->osmData().tagValue(QStringLiteral("colour"));
        return color.isEmpty() ? QStringLiteral("white") : color;
    } else if (role == TextColor) {
        auto const colorValue = m_relations.at(index.row())->osmData().tagValue(QStringLiteral("colour"));
        auto const color = QColor(colorValue.isEmpty() ? QStringLiteral("white") : colorValue);
        return color.valueF() > 0.85 ? QStringLiteral("black") : QStringLiteral("white");
    } else if (role == RouteFrom) {
        return m_relations.at(index.row())->osmData().tagValue(QStringLiteral("from"));
    } else if (role == RouteTo) {
        return m_relations.at(index.row())->osmData().tagValue(QStringLiteral("to"));
    } else if (role == RouteRef) {
        auto const ref = m_relations.at(index.row())->osmData().tagValue(QStringLiteral("ref"));
        return ref.isEmpty() ? m_relations.at(index.row())->name() : ref;
    } else if (role == RouteVia) {
        auto const viaValue = m_relations.at(index.row())->osmData().tagValue(QStringLiteral("via"));
        auto viaList = viaValue.split(';', QString::SkipEmptyParts);
        for (auto &via: viaList) {
            via = via.trimmed();
        }
        return viaList;
    }

    return QVariant();
}

QHash<int, QByteArray> RouteRelationModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[Qt::DisplayRole] = "display";
    roles[IconSource] = "iconSource";
    roles[Description] = "description";
    roles[Network] = "network";
    roles[RouteColor] = "routeColor";
    roles[TextColor] = "textColor";
    roles[RouteFrom] = "routeFrom";
    roles[RouteTo] = "routeTo";
    roles[RouteRef] = "routeRef";
    roles[RouteVia] = "routeVia";
    return roles;
}

QString RouteRelationModel::svgFile(const QString &path) const
{
#ifdef Q_OS_ANDROID
    return MarbleDirs::path(QStringLiteral("svg/%1").arg(path));
#else
    return QStringLiteral("file:///") + MarbleDirs::path(QStringLiteral("svg/%1").arg(path));
#endif
}

}
