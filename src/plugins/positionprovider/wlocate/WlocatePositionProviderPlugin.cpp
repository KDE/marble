//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2012        Dennis Nienhüser <earthwings@gentoo.org>
//

#include "WlocatePositionProviderPlugin.h"

#include <QtCore/QFutureWatcher>
#include <QtCore/QtConcurrentRun>

#ifndef ENV_LINUX
#define ENV_LINUX
#define MARBLE_NEEDS_TO_CLEANUP_FOREIGN_WLOCATE_DEFINE
#include <libwlocate.h>
#endif

#ifdef MARBLE_NEEDS_TO_CLEANUP_FOREIGN_WLOCATE_DEFINE
#undef MARBLE_NEEDS_TO_CLEANUP_FOREIGN_WLOCATE_DEFINE
#undef ENV_LINUX
#endif

namespace Marble {

class WlocatePositionProviderPluginPrivate
{
public:
    PositionProviderStatus m_status;
    qreal m_speed;
    qreal m_direction;
    QDateTime m_timestamp;
    double m_longitude;
    double m_latitude;
    bool m_initialized;
    char m_quality;
    short m_ccode;
    QFutureWatcher<int>* m_futureWatcher;

    WlocatePositionProviderPluginPrivate();

    ~WlocatePositionProviderPluginPrivate();
};

WlocatePositionProviderPluginPrivate::WlocatePositionProviderPluginPrivate() :
        m_status( PositionProviderStatusAcquiring ), m_speed( 0 ), m_direction( 0 ),
        m_longitude( 0.0 ), m_latitude( 0.0 ), m_initialized( false ),
        m_quality( 0 ), m_ccode( 0 ), m_futureWatcher( 0 )
{
    // nothing to do
}

WlocatePositionProviderPluginPrivate::~WlocatePositionProviderPluginPrivate()
{
    // nothing to do
}

QString WlocatePositionProviderPlugin::name() const
{
    return tr( "Wlocate Position Provider Plugin" );
}

QString WlocatePositionProviderPlugin::nameId() const
{
    return "WlocatePositionProvider";
}

QString WlocatePositionProviderPlugin::guiString() const
{
    return tr( "WLAN (Open WLAN Map)" );
}

QString WlocatePositionProviderPlugin::version() const
{
    return "1.0";
}

QString WlocatePositionProviderPlugin::description() const
{
    return tr( "Reports the current position based on nearby WLAN access points" );
}

QString WlocatePositionProviderPlugin::copyrightYears() const
{
    return "2012";
}

QList<PluginAuthor> WlocatePositionProviderPlugin::pluginAuthors() const
{
    return QList<PluginAuthor>()
            << PluginAuthor( QString::fromUtf8( "Dennis Nienhüser" ), "earthwings@gentoo.org" );
}

QIcon WlocatePositionProviderPlugin::icon() const
{
    return QIcon();
}

PositionProviderPlugin* WlocatePositionProviderPlugin::newInstance() const
{
    return new WlocatePositionProviderPlugin;
}

PositionProviderStatus WlocatePositionProviderPlugin::status() const
{
    return d->m_status;
}

GeoDataCoordinates WlocatePositionProviderPlugin::position() const
{
    return GeoDataCoordinates( d->m_longitude, d->m_latitude, 0.0, GeoDataCoordinates::Degree );
}

qreal WlocatePositionProviderPlugin::speed() const
{
    return d->m_speed;
}

qreal WlocatePositionProviderPlugin::direction() const
{
    return d->m_direction;
}

GeoDataAccuracy WlocatePositionProviderPlugin::accuracy() const
{
    GeoDataAccuracy result;

    if ( status() == PositionProviderStatusAvailable ) {
        result.level = GeoDataAccuracy::Detailed;
        /** @todo: Try mapping the accuracy percentage returned by libwlocate to something useful */
        result.horizontal = 0;
        result.vertical = 0;
    }
    else {
        result.level = GeoDataAccuracy::none;
        result.horizontal = 0;
        result.vertical = 0;
    }

    return result;
}

QDateTime WlocatePositionProviderPlugin::timestamp() const
{
    return d->m_timestamp;
}

WlocatePositionProviderPlugin::WlocatePositionProviderPlugin() :
        d( new WlocatePositionProviderPluginPrivate )
{
    // nothing to do
}

WlocatePositionProviderPlugin::~WlocatePositionProviderPlugin()
{
    delete d;
}

void WlocatePositionProviderPlugin::initialize()
{
    d->m_initialized = true;
    update();
}

bool WlocatePositionProviderPlugin::isInitialized() const
{
    return d->m_initialized;
}

void WlocatePositionProviderPlugin::update()
{
    if ( !d->m_futureWatcher ) {
        d->m_futureWatcher = new QFutureWatcher<int>( this );
        connect( d->m_futureWatcher, SIGNAL(finished()), this, SLOT(handleWlocateResult()) );
    }

    QFuture<int> future = QtConcurrent::run( &wloc_get_location, &d->m_latitude, &d->m_longitude, &d->m_quality, &d->m_ccode );
    d->m_futureWatcher->setFuture( future );
}

void WlocatePositionProviderPlugin::handleWlocateResult()
{
    if ( d->m_futureWatcher->isFinished() ) {
        int const returnCode = d->m_futureWatcher->result();
        d->m_status = returnCode == WLOC_OK ? PositionProviderStatusAvailable : PositionProviderStatusError;
        if ( d->m_status == PositionProviderStatusAvailable ) {
#if QT_VERSION < 0x040700
            d->m_timestamp = QDateTime::currentDateTime().toUTC();
#else
            d->m_timestamp = QDateTime::currentDateTimeUtc();
#endif
            emit positionChanged( position(), accuracy() );
        }
    }
}

} // namespace Marble

Q_EXPORT_PLUGIN2( Marble::WlocatePositionProviderPlugin, Marble::WlocatePositionProviderPlugin )

#include "WlocatePositionProviderPlugin.moc"
