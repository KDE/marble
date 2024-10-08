// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2019 Torsten Rahn <rahn@kde.org>
//

#include "GeoItem.h"
#include "MarbleQuickItem.h"

using Marble::GeoDataCoordinates;

namespace Marble
{
GeoItem::GeoItem(QQuickItem *parent)
    : QQuickItem(parent)
    , m_observable(false)
    , m_visible(true)
{
    connect(this, &QQuickItem::parentChanged, this, &GeoItem::setMapToParentOnInit);
    connect(this, &QQuickItem::widthChanged, this, &GeoItem::updateScreenPosition);
    connect(this, &QQuickItem::heightChanged, this, &GeoItem::updateScreenPosition);
}

bool GeoItem::moveToScreenCoordinates(qreal x, qreal y)
{
    bool valid = m_map->screenCoordinatesToGeoDataCoordinates(QPoint(x, y), m_coordinate);
    if (valid) {
        updateScreenPosition();
        Q_EMIT longitudeChanged();
        Q_EMIT latitudeChanged();
    }
    return valid;
}

qreal GeoItem::longitude() const
{
    return m_coordinate.longitude(GeoDataCoordinates::Degree);
}

void GeoItem::setLongitude(qreal lon)
{
    if (m_coordinate.longitude(GeoDataCoordinates::Degree) != lon) {
        m_coordinate.setLongitude(lon, GeoDataCoordinates::Degree);
        updateScreenPosition();
        Q_EMIT longitudeChanged();
    }
}

qreal GeoItem::latitude() const
{
    return m_coordinate.latitude(GeoDataCoordinates::Degree);
}

void GeoItem::setLatitude(qreal lat)
{
    if (m_coordinate.latitude(GeoDataCoordinates::Degree) != lat) {
        m_coordinate.setLatitude(lat, GeoDataCoordinates::Degree);
        updateScreenPosition();
        Q_EMIT latitudeChanged();
    }
}

qreal GeoItem::altitude() const
{
    return m_coordinate.altitude();
}

void GeoItem::setAltitude(qreal alt)
{
    if (m_coordinate.altitude() != alt) {
        m_coordinate.setAltitude(alt);
        updateScreenPosition();
        Q_EMIT altitudeChanged();
    }
}

GeoDataCoordinates GeoItem::coordinates() const
{
    return m_coordinate;
}

void GeoItem::setCoordinates(const GeoDataCoordinates &coordinates)
{
    if (m_coordinate != coordinates) {
        m_coordinate = coordinates;
        updateScreenPosition();
    }
}

MarbleQuickItem *GeoItem::map() const
{
    return m_map;
}

void GeoItem::setMap(MarbleQuickItem *map)
{
    if (m_map == map)
        return;

    m_map = map;

    connect(m_map, &MarbleQuickItem::geoItemUpdateRequested, this, &GeoItem::updateScreenPosition);
    Q_EMIT mapChanged(m_map);
}

void GeoItem::updateScreenPosition()
{
    if (m_map) {
        QPointF relativePoint = m_map->screenCoordinatesFromGeoDataCoordinates(m_coordinate);
        bool observable = !relativePoint.isNull();
        if (observable != m_observable) {
            m_observable = observable;
            Q_EMIT observableChanged(m_observable);
        }
        if (!m_coordinate.isValid()) {
            setPosition(QPointF(-childrenRect().width(), -childrenRect().height()));
        } else if (observable) {
            setPosition(QPointF(0.0, 0.0));
            QPointF screenPoint = mapFromItem(m_map, relativePoint);
            screenPoint -= QPointF(width() / 2.0, height() / 2.0);
            setPosition(screenPoint);
            Q_EMIT readonlyXChanged(readonlyX());
            Q_EMIT readonlyYChanged(readonlyY());
        }
        QQuickItem::setVisible(m_visible && m_observable);
    }
}

void GeoItem::setMapToParentOnInit()
{
    auto visualParent = qobject_cast<MarbleQuickItem *>(parentItem());
    if (visualParent) {
        disconnect(this, &QQuickItem::parentChanged, this, &GeoItem::setMapToParentOnInit);
        setMap(visualParent);
    }
}

bool GeoItem::observable() const
{
    return m_observable;
}

bool GeoItem::visObservable() const
{
    return m_visible;
}

void GeoItem::setVisObservable(bool visible)
{
    if (m_visible == visible)
        return;

    m_visible = visible;
    QQuickItem::setVisible(m_visible && m_observable);
    Q_EMIT visObservableChanged(m_visible);
}
}

#include "moc_GeoItem.cpp"
