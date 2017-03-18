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
#include "GeoDataColorStyle.h"

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
        for (auto relation: relations) {
            if (relation->relationType() >= GeoDataRelation::RouteRoad && relation->relationType() <= GeoDataRelation::RouteSled) {
                m_relations << new GeoDataRelation(*relation);
            }
        }
        std::sort(m_relations.begin(), m_relations.end(),
        [](const GeoDataRelation * a, const GeoDataRelation * b) {
            return *a < *b;
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
        case GeoDataRelation::RouteRoad:         return QStringLiteral("material/directions-car.svg");
        case GeoDataRelation::RouteDetour:       return QStringLiteral("material/directions-car.svg");
        case GeoDataRelation::RouteFerry:        return QStringLiteral("material/directions-boat.svg");
        case GeoDataRelation::RouteTrain:        return QStringLiteral("material/directions-railway.svg");
        case GeoDataRelation::RouteSubway:       return QStringLiteral("material/directions-subway.svg");
        case GeoDataRelation::RouteTram:         return QStringLiteral("material/directions-tram.svg");
        case GeoDataRelation::RouteBus:          return QStringLiteral("material/directions-bus.svg");
        case GeoDataRelation::RouteTrolleyBus:   return QStringLiteral("material/directions-bus.svg");
        case GeoDataRelation::RouteBicycle:      return QStringLiteral("material/directions-bike.svg");
        case GeoDataRelation::RouteMountainbike: return QStringLiteral("material/directions-bike.svg");
        case GeoDataRelation::RouteFoot:         return QStringLiteral("material/directions-walk.svg");
        case GeoDataRelation::RouteHiking:       return QStringLiteral("thenounproject/204712-hiker.svg");
        case GeoDataRelation::RouteHorse:        return QStringLiteral("thenounproject/78374-horse-riding.svg");
        case GeoDataRelation::RouteInlineSkates: return QStringLiteral("thenounproject/101965-inline-skater.svg");
        case GeoDataRelation::RouteSkiDownhill:  return QStringLiteral("thenounproject/2412-skiing-downhill.svg");
        case GeoDataRelation::RouteSkiNordic:    return QStringLiteral("thenounproject/30231-skiing-cross-country.svg");
        case GeoDataRelation::RouteSkitour:      return QStringLiteral("thenounproject/29366-skitour.svg");
        case GeoDataRelation::RouteSled:         return QStringLiteral("thenounproject/365217-sled.svg");
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
        return GeoDataColorStyle::contrastColor(color);
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
    } else if (role == OsmId) {
        return m_relations.at(index.row())->osmData().oid();
    } else if (role == RouteVisible) {
        return m_relations.at(index.row())->isVisible();
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
    roles[OsmId] = "oid";
    roles[RouteVisible] = "routeVisible";
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
