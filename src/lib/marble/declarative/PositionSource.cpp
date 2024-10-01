// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2011 Dennis Nienhüser <nienhueser@kde.org>
// SPDX-FileCopyrightText: 2011 Bernhard Beschow <bbeschow@cs.tu-berlin.de>
//

#include "PositionSource.h"

#include "MarbleModel.h"
#include "MarbleQuickItem.h"
#include "PluginManager.h"
#include "PositionProviderPlugin.h"
#include "PositionTracking.h"

namespace Marble
{

PositionSource::PositionSource(QObject *parent)
    : QObject(parent)
    , m_active(false)
    , m_hasPosition(false)
    , m_position(0)
    , m_marbleQuickItem(nullptr)
    , m_speed(0.0)
{
    // nothing to do
}

PositionSource::~PositionSource() = default;

bool PositionSource::active() const
{
    return m_active;
}

void PositionSource::setActive(bool active)
{
    if (active != m_active) {
        if (active) {
            start();
        } else if (m_marbleQuickItem) {
            PositionTracking *tracking = m_marbleQuickItem->model()->positionTracking();
            tracking->setPositionProviderPlugin(nullptr);
        }

        if (m_hasPosition) {
            m_hasPosition = false;
            Q_EMIT hasPositionChanged();
        }

        m_active = active;
        Q_EMIT activeChanged();
    }
}

QString PositionSource::source() const
{
    return m_source;
}

void PositionSource::setSource(const QString &source)
{
    if (source != m_source) {
        m_source = source;
        if (m_hasPosition) {
            m_hasPosition = false;
            Q_EMIT hasPositionChanged();
        }

        if (active()) {
            start();
        }
        Q_EMIT sourceChanged();
    }
}

bool PositionSource::hasPosition() const
{
    return m_hasPosition;
}

Coordinate *PositionSource::position()
{
    return &m_position;
}

void PositionSource::start()
{
    if (!m_marbleQuickItem) {
        return;
    }

    const PluginManager *pluginManager = m_marbleQuickItem->model()->pluginManager();
    for (const Marble::PositionProviderPlugin *plugin : pluginManager->positionProviderPlugins()) {
        if (m_source.isEmpty() || plugin->nameId() == m_source) {
            PositionProviderPlugin *instance = plugin->newInstance();
            PositionTracking *tracking = m_marbleQuickItem->model()->positionTracking();
            tracking->setPositionProviderPlugin(instance);
            break;
        }
    }
}

MarbleQuickItem *PositionSource::map()
{
    return m_marbleQuickItem;
}

void PositionSource::setMap(MarbleQuickItem *map)
{
    if (map != m_marbleQuickItem) {
        m_marbleQuickItem = map;

        if (m_marbleQuickItem) {
            connect(m_marbleQuickItem->model()->positionTracking(), SIGNAL(gpsLocation(GeoDataCoordinates, qreal)), this, SLOT(updatePosition()));
            connect(m_marbleQuickItem->model()->positionTracking(), SIGNAL(statusChanged(PositionProviderStatus)), this, SLOT(updatePosition()));

            Q_EMIT mapChanged();
        }

        if (active()) {
            start();
        }
    }
}

qreal PositionSource::speed() const
{
    return m_speed;
}

void PositionSource::updatePosition()
{
    if (m_marbleQuickItem) {
        bool const hasPosition = m_marbleQuickItem->model()->positionTracking()->status() == Marble::PositionProviderStatusAvailable;

        if (hasPosition) {
            Marble::GeoDataCoordinates position = m_marbleQuickItem->model()->positionTracking()->currentLocation();
            m_position.setLongitude(position.longitude(Marble::GeoDataCoordinates::Degree));
            m_position.setLatitude(position.latitude(Marble::GeoDataCoordinates::Degree));
            m_position.setAltitude(position.altitude());
        }

        m_speed = m_marbleQuickItem->model()->positionTracking()->speed() * Marble::METER2KM / Marble::SEC2HOUR;
        Q_EMIT speedChanged();

        if (hasPosition != m_hasPosition) {
            m_hasPosition = hasPosition;
            Q_EMIT hasPositionChanged();
        }

        if (hasPosition) {
            Q_EMIT positionChanged();
        }
    }
}

}

#include "moc_PositionSource.cpp"
