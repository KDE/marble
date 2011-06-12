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
    ~QtMobilityPositionProviderPluginPrivate();
    QGeoPositionInfoSource *source();
    const PositionProviderStatus& status() const;
    void setStatus( const PositionProviderStatus& newStatus );

private:
    QGeoPositionInfoSource *m_source;
    PositionProviderStatus m_status;
};

QtMobilityPositionProviderPluginPrivate::QtMobilityPositionProviderPluginPrivate() :
        m_source( 0 ), m_status( PositionProviderStatusAcquiring )
{
    m_source = QGeoPositionInfoSource::createDefaultSource( 0 );
}

QtMobilityPositionProviderPluginPrivate::~QtMobilityPositionProviderPluginPrivate()
{
}

QGeoPositionInfoSource *QtMobilityPositionProviderPluginPrivate::source()
{
    return m_source;
}


const PositionProviderStatus& QtMobilityPositionProviderPluginPrivate::status() const
{
    return m_status;
}

void QtMobilityPositionProviderPluginPrivate::setStatus( const PositionProviderStatus& newStatus )
{
    m_status = newStatus;
}

QString QtMobilityPositionProviderPlugin::name() const
{
    return "Qt Mobility Position Provider Plugin"; /** @todo FIXME I18N ... */
}

QString QtMobilityPositionProviderPlugin::nameId() const
{
    return "QtMobilityPositionProviderPlugin";
}

QString QtMobilityPositionProviderPlugin::guiString() const
{
    return "QtMobilityPositionProviderPlugin"; /** @todo FIXME I18N ... */
}

QString QtMobilityPositionProviderPlugin::description() const
{
    return "Reports the GPS position of a QtMobility compatible device."; /** @todo FIXME I18N ... */
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
    return d->status();
}

GeoDataCoordinates QtMobilityPositionProviderPlugin::position() const
{
    QGeoCoordinate p = d->source()->lastKnownPosition().coordinate();
    if( p.isValid() ) {
        return GeoDataCoordinates( p.longitude(), p.latitude(),
                                   p.altitude(), GeoDataCoordinates::Degree );
    }
    return GeoDataCoordinates();
}

GeoDataAccuracy QtMobilityPositionProviderPlugin::accuracy() const
{
    GeoDataAccuracy result;
    QGeoPositionInfo info = d->source()->lastKnownPosition();

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
    if( d->source() ) {
        connect( d->source(), SIGNAL( positionUpdated ( const QGeoPositionInfo& ) ), this, SLOT( update() ) );
        d->source()->setUpdateInterval( 1000 );
        d->source()->startUpdates();
    }
}

bool QtMobilityPositionProviderPlugin::isInitialized() const
{
    return d->source() != 0;
}

qreal QtMobilityPositionProviderPlugin::speed() const
{
    if( d->source()->lastKnownPosition().hasAttribute( QGeoPositionInfo::GroundSpeed ) ) {
        return d->source()->lastKnownPosition().attribute( QGeoPositionInfo::GroundSpeed );
    }
    return 0.0;
}

qreal QtMobilityPositionProviderPlugin::direction() const
{
    if( d->source()->lastKnownPosition().hasAttribute( QGeoPositionInfo::Direction ) ) {
        return d->source()->lastKnownPosition().attribute( QGeoPositionInfo::Direction );
    }
    return 0.0;
}

void QtMobilityPositionProviderPlugin::update()
{
    PositionProviderStatus newStatus = PositionProviderStatusAcquiring;
    if ( d->source() ) {
        if ( d->source()->lastKnownPosition().isValid() ) {
            newStatus = PositionProviderStatusAvailable;
        }
        else {
            newStatus = PositionProviderStatusUnavailable;
        }
    }

    if ( newStatus != d->status() ) {
        d->setStatus( newStatus );
        emit statusChanged( newStatus );
    }

    if ( newStatus == PositionProviderStatusAvailable ) {
        emit positionChanged( position(), accuracy() );
    }
}

} // namespace Marble

Q_EXPORT_PLUGIN2( Marble::QtMobilityPositionProviderPlugin, Marble::QtMobilityPositionProviderPlugin )

#include "QtMobilityPositionProviderPlugin.moc"
