// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2011 Dennis Nienhüser <nienhueser@kde.org>
//

#include "Tracking.h"

#include "AutoNavigation.h"
#include "MarbleModel.h"
#include "MarbleQuickItem.h"
#include "PositionTracking.h"
#include "RenderPlugin.h"
#include "ViewportParams.h"

namespace Marble
{

Tracking::Tracking(QObject *parent)
    : QObject(parent)
    , m_showTrack(true)
    , m_hasLastKnownPosition(false)
    , m_positionMarkerType(None)
{
    connect(&m_lastKnownPosition, &Coordinate::longitudeChanged, this, &Tracking::setHasLastKnownPosition);
    connect(&m_lastKnownPosition, &Coordinate::latitudeChanged, this, &Tracking::setHasLastKnownPosition);
}

bool Tracking::showTrack() const
{
    return m_showTrack;
}

void Tracking::setShowTrack(bool show)
{
    if (show != m_showTrack) {
        if (m_marbleQuickItem) {
            m_marbleQuickItem->model()->positionTracking()->setTrackVisible(show);
            m_marbleQuickItem->update();
        }

        m_showTrack = show;
        Q_EMIT showTrackChanged();
    }
}

PositionSource *Tracking::positionSource()
{
    return m_positionSource;
}

void Tracking::setPositionSource(PositionSource *source)
{
    if (source != m_positionSource) {
        m_positionSource = source;
        if (source) {
            connect(source, SIGNAL(positionChanged()), this, SLOT(updatePositionMarker()));
            connect(source, SIGNAL(positionChanged()), this, SLOT(updateLastKnownPosition()));
            connect(source, SIGNAL(hasPositionChanged()), this, SLOT(updatePositionMarker()));
            connect(source, SIGNAL(positionChanged()), this, SIGNAL(distanceChanged()));
        }
        Q_EMIT positionSourceChanged();
    }
}

MarbleQuickItem *Tracking::map()
{
    return m_marbleQuickItem;
}

void Tracking::setMap(MarbleQuickItem *item)
{
    if (item != m_marbleQuickItem) {
        m_marbleQuickItem = item;

        if (m_marbleQuickItem) {
            m_marbleQuickItem->model()->positionTracking()->setTrackVisible(showTrack());
            setShowPositionMarkerPlugin(m_positionMarkerType == Arrow);

            connect(m_marbleQuickItem, SIGNAL(visibleLatLonAltBoxChanged()), this, SLOT(updatePositionMarker()));
            connect(m_marbleQuickItem, SIGNAL(mapThemeChanged()), this, SLOT(updatePositionMarker()));
        }

        Q_EMIT mapChanged();
    }
}

void Tracking::setPositionMarker(QObject *marker)
{
    if (marker != m_positionMarker) {
        m_positionMarker = marker;
        Q_EMIT positionMarkerChanged();
    }
}

QObject *Tracking::positionMarker()
{
    return m_positionMarker;
}

void Tracking::updatePositionMarker()
{
    if (m_marbleQuickItem && m_positionMarker && m_positionMarkerType == Circle) {
        Coordinate *position = nullptr;
        bool visible = (m_marbleQuickItem->model()->planetId() == QLatin1StringView("earth"));
        if (m_positionSource && m_positionSource->hasPosition()) {
            position = m_positionSource->position();
        } else if (hasLastKnownPosition()) {
            position = lastKnownPosition();
        } else {
            visible = false;
        }

        qreal x(0), y(0);
        if (position) {
            Marble::GeoDataCoordinates const pos(position->longitude(), position->latitude(), 0.0, GeoDataCoordinates::Degree);
            visible = visible && m_marbleQuickItem->map()->viewport()->screenCoordinates(pos.longitude(), pos.latitude(), x, y);
            QQuickItem *item = qobject_cast<QQuickItem *>(m_positionMarker);
            if (item) {
                item->setVisible(visible);
                if (visible) {
                    item->setX(x - item->width() / 2.0);
                    item->setY(y - item->height() / 2.0);
                }
            }
        }
    } else if (m_positionMarkerType != Circle) {
        QQuickItem *item = qobject_cast<QQuickItem *>(m_positionMarker);
        if (item) {
            item->setVisible(false);
        }
    }
}

void Tracking::updateLastKnownPosition()
{
    if (m_positionSource && m_positionSource->hasPosition()) {
        setLastKnownPosition(m_positionSource->position());
    }
}

void Tracking::setHasLastKnownPosition()
{
    if (!m_hasLastKnownPosition) {
        m_hasLastKnownPosition = true;
        Q_EMIT hasLastKnownPositionChanged();
    }
}

void Tracking::setShowPositionMarkerPlugin(bool visible)
{
    if (m_marbleQuickItem) {
        QList<RenderPlugin *> const renderPlugins = m_marbleQuickItem->map()->renderPlugins();
        for (RenderPlugin *renderPlugin : renderPlugins) {
            Q_ASSERT(renderPlugin);
            if (renderPlugin->nameId() == QLatin1StringView("positionMarker")) {
                renderPlugin->setEnabled(true);
                renderPlugin->setVisible(visible);
            }
        }
    }
}

bool Tracking::hasLastKnownPosition() const
{
    return m_hasLastKnownPosition;
}

Coordinate *Tracking::lastKnownPosition()
{
    return &m_lastKnownPosition;
}

void Tracking::setLastKnownPosition(Coordinate *lastKnownPosition)
{
    if (lastKnownPosition && *lastKnownPosition != m_lastKnownPosition) {
        m_lastKnownPosition.setCoordinates(lastKnownPosition->coordinates());
        Q_EMIT lastKnownPositionChanged();
    }
}

bool Tracking::autoCenter() const
{
    if (m_autoNavigation) {
        return m_autoNavigation->recenterMode() != Marble::AutoNavigation::DontRecenter;
    }

    return false;
}

void Tracking::setAutoCenter(bool enabled)
{
    if (autoCenter() != enabled) {
        if (enabled && !m_autoNavigation && m_marbleQuickItem) {
            m_autoNavigation = new Marble::AutoNavigation(m_marbleQuickItem->model(), m_marbleQuickItem->map()->viewport(), this);
            connect(m_autoNavigation, SIGNAL(zoomIn(FlyToMode)), m_marbleQuickItem, SLOT(zoomIn()));
            connect(m_autoNavigation, SIGNAL(zoomOut(FlyToMode)), m_marbleQuickItem, SLOT(zoomOut()));
            connect(m_autoNavigation, SIGNAL(centerOn(GeoDataCoordinates, bool)), m_marbleQuickItem, SLOT(centerOn(GeoDataCoordinates)));

            connect(m_marbleQuickItem, SIGNAL(visibleLatLonAltBoxChanged()), m_autoNavigation, SLOT(inhibitAutoAdjustments()));
        }

        if (m_autoNavigation) {
            m_autoNavigation->setRecenter(Marble::AutoNavigation::RecenterOnBorder);
        }

        Q_EMIT autoCenterChanged();
    }
}

bool Tracking::autoZoom() const
{
    if (m_autoNavigation) {
        return m_autoNavigation->autoZoom();
    }

    return false;
}

void Tracking::setAutoZoom(bool enabled)
{
    if (autoZoom() != enabled) {
        if (enabled && !m_autoNavigation && m_marbleQuickItem) {
            m_autoNavigation = new Marble::AutoNavigation(m_marbleQuickItem->model(), m_marbleQuickItem->map()->viewport(), this);
            connect(m_autoNavigation, SIGNAL(zoomIn(FlyToMode)), m_marbleQuickItem, SLOT(zoomIn()));
            connect(m_autoNavigation, SIGNAL(zoomOut(FlyToMode)), m_marbleQuickItem, SLOT(zoomOut()));
            connect(m_autoNavigation, SIGNAL(centerOn(GeoDataCoordinates, bool)), m_marbleQuickItem, SLOT(centerOn(GeoDataCoordinates)));

            connect(m_marbleQuickItem, SIGNAL(visibleLatLonAltBoxChanged()), m_autoNavigation, SLOT(inhibitAutoAdjustments()));
        }

        if (m_autoNavigation) {
            m_autoNavigation->setAutoZoom(enabled);
        }

        Q_EMIT autoZoomChanged();
    }
}

Tracking::PositionMarkerType Tracking::positionMarkerType() const
{
    return m_positionMarkerType;
}

void Tracking::setPositionMarkerType(Tracking::PositionMarkerType type)
{
    setShowPositionMarkerPlugin(type == Arrow);
    if (type != m_positionMarkerType) {
        m_positionMarkerType = type;
        Q_EMIT positionMarkerTypeChanged();
    }
}

double Tracking::distance() const
{
    return m_marbleQuickItem ? m_marbleQuickItem->model()->positionTracking()->length(m_marbleQuickItem->model()->planetRadius()) : 0.0;
}

void Tracking::saveTrack(const QString &fileName)
{
    if (m_marbleQuickItem) {
        /** @todo FIXME: replace the file:// prefix on QML side */
        QString target = fileName.startsWith(QLatin1StringView("file://")) ? fileName.mid(7) : fileName;
        m_marbleQuickItem->model()->positionTracking()->saveTrack(target);
    }
}

void Tracking::openTrack(const QString &fileName)
{
    if (m_marbleQuickItem) {
        /** @todo FIXME: replace the file:// prefix on QML side */
        QString target = fileName.startsWith(QLatin1StringView("file://")) ? fileName.mid(7) : fileName;
        m_marbleQuickItem->model()->addGeoDataFile(target);
    }
}

void Tracking::clearTrack()
{
    if (m_marbleQuickItem) {
        m_marbleQuickItem->model()->positionTracking()->clearTrack();
    }
}

}

#include "moc_Tracking.cpp"
