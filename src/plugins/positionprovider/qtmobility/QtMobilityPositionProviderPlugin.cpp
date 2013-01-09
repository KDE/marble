//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011        Daniel Marth <danielmarth@gmx.at>
// Copyright 2012        Bernhard Beschow <bbeschow@cs.tu-berlin.de>
//

#include "QtMobilityPositionProviderPlugin.h"

#include <QtLocation/QGeoPositionInfoSource>
#include <QtLocation/QGeoPositionInfo>
#include <QtLocation/QGeoCoordinate>

QTM_USE_NAMESPACE

namespace Marble {

class QtMobilityPositionProviderPluginPrivate
{
public:
    QtMobilityPositionProviderPluginPrivate();

    QGeoPositionInfoSource *const m_source;
    PositionProviderStatus m_status;
};

QtMobilityPositionProviderPluginPrivate::QtMobilityPositionProviderPluginPrivate() :
    m_source( QGeoPositionInfoSource::createDefaultSource( 0 ) ),
    m_status( PositionProviderStatusAcquiring )
{
}

QString QtMobilityPositionProviderPlugin::name() const
{
    return tr( "Qt Mobility Position Provider Plugin" );
}

QString QtMobilityPositionProviderPlugin::nameId() const
{
    return "QtMobilityPositionProviderPlugin";
}

QString QtMobilityPositionProviderPlugin::guiString() const
{
    return tr( "Qt Mobility Location" );
}

QString QtMobilityPositionProviderPlugin::version() const
{
    return "1.0";
}

QString QtMobilityPositionProviderPlugin::description() const
{
    return tr( "Reports the GPS position of a QtMobility compatible device." );
}

QString QtMobilityPositionProviderPlugin::copyrightYears() const
{
    return "2011";
}

QList<PluginAuthor> QtMobilityPositionProviderPlugin::pluginAuthors() const
{
    return QList<PluginAuthor>()
            << PluginAuthor( "Daniel Marth", "danielmarth@gmx.at" );
}

QIcon QtMobilityPositionProviderPlugin::icon() const
{
    return QIcon();
}

PositionProviderPlugin* QtMobilityPositionProviderPlugin::newInstance() const
{
    return new QtMobilityPositionProviderPlugin;
}

PositionProviderStatus QtMobilityPositionProviderPlugin::status() const
{
    return d->m_status;
}

GeoDataCoordinates QtMobilityPositionProviderPlugin::position() const
{
    if ( d->m_source == 0 ) {
        return GeoDataCoordinates();
    }

    const QGeoCoordinate p = d->m_source->lastKnownPosition().coordinate();
    if( !p.isValid() ) {
        return GeoDataCoordinates();
    }

    return GeoDataCoordinates( p.longitude(), p.latitude(),
                               p.altitude(), GeoDataCoordinates::Degree );
}

GeoDataAccuracy QtMobilityPositionProviderPlugin::accuracy() const
{
    if ( d->m_source == 0 ) {
        return GeoDataAccuracy();
    }

    const QGeoPositionInfo info = d->m_source->lastKnownPosition();

    if( !info.hasAttribute( QGeoPositionInfo::HorizontalAccuracy ) ||
        !info.hasAttribute( QGeoPositionInfo::VerticalAccuracy ) ) {
        return GeoDataAccuracy();
    }

    const qreal horizontal = info.attribute( QGeoPositionInfo::HorizontalAccuracy );
    const qreal vertical = info.attribute( QGeoPositionInfo::VerticalAccuracy );

    return GeoDataAccuracy( GeoDataAccuracy::Detailed, horizontal, vertical );
}

QtMobilityPositionProviderPlugin::QtMobilityPositionProviderPlugin() :
        d( new QtMobilityPositionProviderPluginPrivate )
{
}

QtMobilityPositionProviderPlugin::~QtMobilityPositionProviderPlugin()
{
    delete d;
}

void QtMobilityPositionProviderPlugin::initialize()
{
    if( d->m_source ) {
        connect( d->m_source, SIGNAL(positionUpdated(QGeoPositionInfo)), this, SLOT(update()) );
        d->m_source->setUpdateInterval( 1000 );
        d->m_source->startUpdates();
    }
}

bool QtMobilityPositionProviderPlugin::isInitialized() const
{
    return d->m_source != 0;
}

qreal QtMobilityPositionProviderPlugin::speed() const
{
    if ( d->m_source == 0 ) {
        return 0.0;
    }

    if( !d->m_source->lastKnownPosition().hasAttribute( QGeoPositionInfo::GroundSpeed ) ) {
        return 0.0;
    }

    return d->m_source->lastKnownPosition().attribute( QGeoPositionInfo::GroundSpeed );
}

qreal QtMobilityPositionProviderPlugin::direction() const
{
    if ( d->m_source == 0 ) {
        return 0.0;
    }

    if( !d->m_source->lastKnownPosition().hasAttribute( QGeoPositionInfo::Direction ) ) {
        return 0.0;
    }

    return d->m_source->lastKnownPosition().attribute( QGeoPositionInfo::Direction );
}

QDateTime QtMobilityPositionProviderPlugin::timestamp() const
{
    if ( d->m_source == 0 ) {
        return QDateTime();
    }

    return d->m_source->lastKnownPosition().timestamp();
}

void QtMobilityPositionProviderPlugin::update()
{
    PositionProviderStatus newStatus = PositionProviderStatusAcquiring;
    if ( d->m_source ) {
        if ( d->m_source->lastKnownPosition().isValid() ) {
            newStatus = PositionProviderStatusAvailable;
        }
        else {
            newStatus = PositionProviderStatusUnavailable;
        }
    }

    if ( newStatus != d->m_status ) {
        d->m_status = newStatus;
        emit statusChanged( newStatus );
    }

    if ( newStatus == PositionProviderStatusAvailable ) {
        emit positionChanged( position(), accuracy() );
    }
}

} // namespace Marble

Q_EXPORT_PLUGIN2( Marble::QtMobilityPositionProviderPlugin, Marble::QtMobilityPositionProviderPlugin )

#include "QtMobilityPositionProviderPlugin.moc"
