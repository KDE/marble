// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2010 Dennis Nienhüser <nienhueser@kde.org>
// SPDX-FileCopyrightText: 2011 Thibaut Gridel <tgridel@free.fr>
// SPDX-FileCopyrightText: 2012 Bernhard Beschow <bbeschow@cs.tu-berlin.de>
//

#include "ReverseGeocodingRunnerPlugin.h"

#include <QIcon>

namespace Marble
{

class Q_DECL_HIDDEN ReverseGeocodingRunnerPlugin::Private
{
public:
    QStringList m_supportedCelestialBodies;

    bool m_canWorkOffline;

    Private();
};

ReverseGeocodingRunnerPlugin::Private::Private()
    : m_canWorkOffline(true)
{
    // nothing to do
}

ReverseGeocodingRunnerPlugin::ReverseGeocodingRunnerPlugin(QObject *parent)
    : QObject(parent)
    , d(new Private)
{
}

ReverseGeocodingRunnerPlugin::~ReverseGeocodingRunnerPlugin()
{
    delete d;
}

QIcon ReverseGeocodingRunnerPlugin::icon() const
{
    return {};
}

bool ReverseGeocodingRunnerPlugin::supportsCelestialBody(const QString &celestialBodyId) const
{
    if (d->m_supportedCelestialBodies.isEmpty()) {
        return true;
    }

    return d->m_supportedCelestialBodies.contains(celestialBodyId);
}

void ReverseGeocodingRunnerPlugin::setSupportedCelestialBodies(const QStringList &celestialBodies)
{
    d->m_supportedCelestialBodies = celestialBodies;
}

void ReverseGeocodingRunnerPlugin::setCanWorkOffline(bool canWorkOffline)
{
    d->m_canWorkOffline = canWorkOffline;
}

bool ReverseGeocodingRunnerPlugin::canWorkOffline() const
{
    return d->m_canWorkOffline;
}

bool ReverseGeocodingRunnerPlugin::canWork() const
{
    return true;
}

}

#include "moc_ReverseGeocodingRunnerPlugin.cpp"
