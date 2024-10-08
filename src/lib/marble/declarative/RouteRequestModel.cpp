// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2011 Dennis Nienhüser <nienhueser@kde.org>
//

#include "RouteRequestModel.h"

#include "MarbleMap.h"
#include "MarbleModel.h"
#include "Routing.h"
#include "routing/RouteRequest.h"
#include "routing/RoutingManager.h"
#include <GeoDataPlacemark.h>

RouteRequestModel::RouteRequestModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

RouteRequestModel::~RouteRequestModel() = default;

int RouteRequestModel::rowCount(const QModelIndex &parent) const
{
    if (!parent.isValid() && m_request) {
        return m_request->size();
    }

    return 0;
}

QHash<int, QByteArray> RouteRequestModel::roleNames() const
{
    return {
        {Qt::DisplayRole, "name"},
        {LongitudeRole, "longitude"},
        {LatitudeRole, "latitude"},
    };
}

QVariant RouteRequestModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole && section == 0) {
        return QStringLiteral("Waypoint");
    }

    return {};
}

QVariant RouteRequestModel::data(const QModelIndex &index, int role) const
{
    Q_ASSERT(checkIndex(index, QAbstractItemModel::CheckIndexOption::IndexIsValid));

    switch (role) {
    case Qt::DisplayRole: {
        Marble::GeoDataPlacemark const &placemark = (*m_request)[index.row()];
        if (!placemark.name().isEmpty()) {
            return placemark.name();
        }

        if (!placemark.address().isEmpty()) {
            return placemark.address();
        }

        return placemark.coordinate().toString(Marble::GeoDataCoordinates::Decimal).trimmed();
    }
    case LongitudeRole:
        return m_request->at(index.row()).longitude(Marble::GeoDataCoordinates::Degree);
    case LatitudeRole:
        return m_request->at(index.row()).latitude(Marble::GeoDataCoordinates::Degree);
    default:
        return {};
    }
}

Marble::Routing *RouteRequestModel::routing()
{
    return m_routing;
}

void RouteRequestModel::setRouting(Marble::Routing *routing)
{
    if (routing == m_routing) {
        return;
    }
    m_routing = routing;
    updateMap();
    connect(m_routing, &Marble::Routing::marbleMapChanged, this, &RouteRequestModel::updateMap);
    Q_EMIT routingChanged();
}

void RouteRequestModel::updateMap()
{
    if (m_routing && m_routing->marbleMap()) {
        m_request = m_routing->marbleMap()->model()->routingManager()->routeRequest();

        connect(m_request, &Marble::RouteRequest::positionChanged, this, &RouteRequestModel::updateData, Qt::UniqueConnection);
        connect(m_request, &Marble::RouteRequest::positionAdded, this, &RouteRequestModel::updateAfterAddition, Qt::UniqueConnection);
        connect(m_request, &Marble::RouteRequest::positionRemoved, this, &RouteRequestModel::updateAfterRemoval, Qt::UniqueConnection);

        Q_EMIT layoutChanged();
    }
}

void RouteRequestModel::updateData(int idx)
{
    QModelIndex affected = index(idx);
    Q_EMIT dataChanged(affected, affected);
}

void RouteRequestModel::updateAfterRemoval(int idx)
{
    beginRemoveRows(QModelIndex(), idx, idx);
    removeRow(idx);
    endRemoveRows();
}

void RouteRequestModel::updateAfterAddition(int idx)
{
    beginInsertRows(QModelIndex(), idx, idx);
    insertRow(idx);
    endInsertRows();
}

void RouteRequestModel::setPosition(int index, qreal longitude, qreal latitude, const QString &name)
{
    if (index >= 0 && index < m_request->size()) {
        m_request->setPosition(index, Marble::GeoDataCoordinates(longitude, latitude, 0.0, Marble::GeoDataCoordinates::Degree), name);
    }
}

void RouteRequestModel::reverse()
{
    beginResetModel();
    m_request->reverse();
    endResetModel();
}

#include "moc_RouteRequestModel.cpp"
