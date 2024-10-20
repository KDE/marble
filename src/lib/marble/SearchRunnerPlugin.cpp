// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2010 Dennis Nienhüser <nienhueser@kde.org>
// SPDX-FileCopyrightText: 2011 Thibaut Gridel <tgridel@free.fr>
// SPDX-FileCopyrightText: 2012 Bernhard Beschow <bbeschow@cs.tu-berlin.de>
//

#include "SearchRunnerPlugin.h"

#include <QIcon>

namespace Marble
{

class Q_DECL_HIDDEN SearchRunnerPlugin::Private
{
public:
    QStringList m_supportedCelestialBodies;

    bool m_canWorkOffline;

    Private();
};

SearchRunnerPlugin::Private::Private()
    : m_canWorkOffline(true)
{
    // nothing to do
}

SearchRunnerPlugin::SearchRunnerPlugin(QObject *parent)
    : QObject(parent)
    , d(new Private)
{
}

SearchRunnerPlugin::~SearchRunnerPlugin()
{
    delete d;
}

QIcon SearchRunnerPlugin::icon() const
{
    return {};
}

bool SearchRunnerPlugin::supportsCelestialBody(const QString &celestialBodyId) const
{
    if (d->m_supportedCelestialBodies.isEmpty()) {
        return true;
    }

    return d->m_supportedCelestialBodies.contains(celestialBodyId);
}

void SearchRunnerPlugin::setSupportedCelestialBodies(const QStringList &celestialBodies)
{
    d->m_supportedCelestialBodies = celestialBodies;
}

void SearchRunnerPlugin::setCanWorkOffline(bool canWorkOffline)
{
    d->m_canWorkOffline = canWorkOffline;
}

bool SearchRunnerPlugin::canWorkOffline() const
{
    return d->m_canWorkOffline;
}

bool SearchRunnerPlugin::canWork() const
{
    return true;
}

}

#include "moc_SearchRunnerPlugin.cpp"
