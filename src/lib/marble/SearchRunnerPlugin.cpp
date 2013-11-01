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

#include "SearchRunnerPlugin.h"

namespace Marble
{

class SearchRunnerPlugin::Private
{
public:
    QStringList m_supportedCelestialBodies;

    bool m_canWorkOffline;

    Private();
};

SearchRunnerPlugin::Private::Private()
    : m_canWorkOffline( true )
{
    // nothing to do
}

SearchRunnerPlugin::SearchRunnerPlugin( QObject* parent ) :
    QObject( parent ),
    d( new Private )
{
}

SearchRunnerPlugin::~SearchRunnerPlugin()
{
    delete d;
}

QIcon SearchRunnerPlugin::icon() const
{
    return QIcon();
}

bool SearchRunnerPlugin::supportsCelestialBody( const QString &celestialBodyId ) const
{
    if ( d->m_supportedCelestialBodies.isEmpty() ) {
        return true;
    }

    return d->m_supportedCelestialBodies.contains( celestialBodyId );
}

void SearchRunnerPlugin::setSupportedCelestialBodies( const QStringList &celestialBodies )
{
    d->m_supportedCelestialBodies = celestialBodies;
}

void SearchRunnerPlugin::setCanWorkOffline( bool canWorkOffline )
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

#include "SearchRunnerPlugin.moc"
