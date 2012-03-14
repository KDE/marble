//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011        Daniel Marth <danielmarth@gmx.at>
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
    QGeoCoordinate p = d->m_source->lastKnownPosition().coordinate();
    if( p.isValid() ) {
        return GeoDataCoordinates( p.longitude(), p.latitude(),
                                   p.altitude(), GeoDataCoordinates::Degree );
    }
    return GeoDataCoordinates();
}

GeoDataAccuracy QtMobilityPositionProviderPlugin::accuracy() const
{
    GeoDataAccuracy result;
    QGeoPositionInfo info = d->m_source->lastKnownPosition();

    if( info.hasAttribute( QGeoPositionInfo::HorizontalAccuracy ) &&
        info.hasAttribute( QGeoPositionInfo::VerticalAccuracy ) ) {
        result.level = GeoDataAccuracy::Detailed;
        result.horizontal = info.attribute( QGeoPositionInfo::HorizontalAccuracy );
        result.vertical = info.attribute( QGeoPositionInfo::VerticalAccuracy );
    }
    else {
        result.level = GeoDataAccuracy::none;
        result.horizontal = 0;
        result.vertical = 0;
    }

    return result;
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
        connect( d->m_source, SIGNAL( positionUpdated ( const QGeoPositionInfo& ) ), this, SLOT( update() ) );
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
    if( d->m_source->lastKnownPosition().hasAttribute( QGeoPositionInfo::GroundSpeed ) ) {
        return d->m_source->lastKnownPosition().attribute( QGeoPositionInfo::GroundSpeed );
    }
    return 0.0;
}

qreal QtMobilityPositionProviderPlugin::direction() const
{
    if( d->m_source->lastKnownPosition().hasAttribute( QGeoPositionInfo::Direction ) ) {
        return d->m_source->lastKnownPosition().attribute( QGeoPositionInfo::Direction );
    }
    return 0.0;
}

QDateTime QtMobilityPositionProviderPlugin::timestamp() const
{
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
