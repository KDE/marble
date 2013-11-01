//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010 Dennis Nienhüser <earthwings@gentoo.org>
// Copyright 2011 Thibaut Gridel <tgridel@free.fr>
// Copyright 2012 Bernhard Beschow <bbeschow@cs.tu-berlin.de>
//

#include "RoutingRunnerPlugin.h"

namespace Marble
{

class RoutingRunnerPlugin::Private
{
public:
    QStringList m_supportedCelestialBodies;

    bool m_canWorkOffline;

    QString m_statusMessage;

    Private();
};

RoutingRunnerPlugin::Private::Private()
    : m_canWorkOffline( true )
{
    // nothing to do
}

RoutingRunnerPlugin::RoutingRunnerPlugin( QObject *parent ) :
    QObject( parent ),
    d( new Private )
{
}

RoutingRunnerPlugin::~RoutingRunnerPlugin()
{
    delete d;
}

QIcon RoutingRunnerPlugin::icon() const
{
    return QIcon();
}

bool RoutingRunnerPlugin::supportsCelestialBody( const QString &celestialBodyId ) const
{
    if ( d->m_supportedCelestialBodies.isEmpty() ) {
        return true;
    }

    return d->m_supportedCelestialBodies.contains( celestialBodyId );
}

void RoutingRunnerPlugin::setSupportedCelestialBodies( const QStringList &celestialBodies )
{
    d->m_supportedCelestialBodies = celestialBodies;
}

void RoutingRunnerPlugin::setCanWorkOffline( bool canWorkOffline )
{
    d->m_canWorkOffline = canWorkOffline;
}

bool RoutingRunnerPlugin::canWorkOffline() const
{
    return d->m_canWorkOffline;
}

bool RoutingRunnerPlugin::canWork() const
{
    return true;
}

RoutingRunnerPlugin::ConfigWidget *RoutingRunnerPlugin::configWidget()
{
    return 0;
}

bool RoutingRunnerPlugin::supportsTemplate( RoutingProfilesModel::ProfileTemplate ) const
{
    return false;
}

QHash< QString, QVariant > RoutingRunnerPlugin::templateSettings( RoutingProfilesModel::ProfileTemplate ) const
{
    return QHash< QString, QVariant >();
}

QString RoutingRunnerPlugin::statusMessage() const
{
    return d->m_statusMessage;
}

void RoutingRunnerPlugin::setStatusMessage( const QString &message )
{
    d->m_statusMessage = message;
}

}

#include "RoutingRunnerPlugin.moc"
