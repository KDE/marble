//
// This file is part of the Marble Virtual Globe.
//
// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2019 Torsten Rahn <rahn@kde.org>
//

#include "GeoItem.h"
#include "MarbleQuickItem.h"
#include "Coordinate.h"

#include "MarbleGlobal.h"

using Marble::GeoDataCoordinates;
using Marble::EARTH_RADIUS;
using Marble::DEG2RAD;

namespace Marble
{
    GeoItem::GeoItem(QQuickItem *parent ) :
        QQuickItem( parent ),
        m_map(nullptr),
        m_observable(false),
        m_visible(true)
    {
        connect(this, &QQuickItem::parentChanged, this, &GeoItem::setMapToParentOnInit);
        connect(this, &QQuickItem::widthChanged, this, &GeoItem::updateScreenPosition);
        connect(this, &QQuickItem::heightChanged, this, &GeoItem::updateScreenPosition);
    }

    bool GeoItem::moveToScreenCoordinates(qreal x, qreal y)
    {
        return m_map->screenCoordinatesToGeoDataCoordinates(QPoint(x,y), m_coordinate);
        updateScreenPosition();
        emit longitudeChanged();
        emit latitudeChanged();
    }

    qreal GeoItem::longitude() const
    {
        return m_coordinate.longitude( GeoDataCoordinates::Degree );
    }

    void GeoItem::setLongitude( qreal lon )
    {
        m_coordinate.setLongitude( lon, GeoDataCoordinates::Degree );
        updateScreenPosition();
        emit longitudeChanged();
    }

    qreal GeoItem::latitude() const
    {
        return m_coordinate.latitude( GeoDataCoordinates::Degree );
    }

    void GeoItem::setLatitude( qreal lat )
    {
        m_coordinate.setLatitude( lat, GeoDataCoordinates::Degree );
        updateScreenPosition();
        emit latitudeChanged();
    }

    qreal GeoItem::altitude() const
    {
        return m_coordinate.altitude();
    }

    void GeoItem::setAltitude( qreal alt )
    {
        m_coordinate.setAltitude( alt );
        updateScreenPosition();
        emit altitudeChanged();
    }

    GeoDataCoordinates GeoItem::coordinates() const
    {
        return m_coordinate;
    }

    void GeoItem::setCoordinates( const GeoDataCoordinates &coordinates )
    {
        m_coordinate = coordinates;
        updateScreenPosition();
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

        connect(m_map, &MarbleQuickItem::visibleLatLonAltBoxChanged, this, [=]() { updateScreenPosition(); });
        emit mapChanged(m_map);
    }

    void GeoItem::updateScreenPosition() {
        if (m_map) {
            QPointF relativePoint = m_map->screenCoordinatesFromGeoDataCoordinates(m_coordinate);
            bool observable = !relativePoint.isNull();
            if (observable != m_observable) {
                m_observable = observable;
                QQuickItem::setVisible(m_visible && m_observable);
                emit observableChanged(m_observable);
            }
            else {
                setPosition(QPointF(0.0,0.0));
                QPointF screenPoint = mapFromItem(m_map, relativePoint);
                screenPoint -= QPointF(width()/2.0, height()/2.0);
                setPosition(screenPoint);
                emit readonlyXChanged(readonlyX()) ;
                emit readonlyYChanged(readonlyY()) ;
            }
        }
    }

    void GeoItem::setMapToParentOnInit()
    {
        MarbleQuickItem * visualParent = qobject_cast<MarbleQuickItem*>(parentItem());
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
        emit visObservableChanged(m_visible);
    }
}

#include "moc_GeoItem.cpp"
