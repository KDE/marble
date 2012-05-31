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

#include "ReverseGeocodingRunnerPlugin.h"

namespace Marble
{

class ReverseGeocodingRunnerPlugin::Private
{
public:
    QStringList m_supportedCelestialBodies;

    bool m_canWorkOffline;

    Private();
};

ReverseGeocodingRunnerPlugin::Private::Private()
    : m_canWorkOffline( true )
{
    // nothing to do
}

ReverseGeocodingRunnerPlugin::ReverseGeocodingRunnerPlugin( QObject* parent ) :
    QObject( parent ),
    d( new Private )
{
}

ReverseGeocodingRunnerPlugin::~ReverseGeocodingRunnerPlugin()
{
    delete d;
}

QIcon ReverseGeocodingRunnerPlugin::icon() const
{
    return QIcon();
}

bool ReverseGeocodingRunnerPlugin::supportsCelestialBody( const QString &celestialBodyId ) const
{
    if ( d->m_supportedCelestialBodies.isEmpty() ) {
        return true;
    }

    return d->m_supportedCelestialBodies.contains( celestialBodyId );
}

void ReverseGeocodingRunnerPlugin::setSupportedCelestialBodies( const QStringList &celestialBodies )
{
    d->m_supportedCelestialBodies = celestialBodies;
}

void ReverseGeocodingRunnerPlugin::setCanWorkOffline( bool canWorkOffline )
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

#include "ReverseGeocodingRunnerPlugin.moc"
