//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011      Guillaume Martres <smarter@ubuntu.com>
// Copyright 2011,2012 Bernhard Beschow <bbeschow@cs.tu-berlin.de>
//

#include "PlacemarkPositionProviderPlugin.h"

#include "GeoDataPlacemark.h"
#include "MarbleClock.h"
#include "MarbleMath.h"
#include "MarbleModel.h"
#include "MarbleDebug.h"

#include <QIcon>

using namespace Marble;

PlacemarkPositionProviderPlugin::PlacemarkPositionProviderPlugin( MarbleModel *marbleModel )
    : PositionProviderPlugin(),
      m_marbleModel( marbleModel ),
      m_placemark( 0 ),
      m_speed( 0 ),
      m_direction( 0.0 ),
      m_status( PositionProviderStatusUnavailable ),
      m_isInitialized( false )
{
    m_accuracy.level = GeoDataAccuracy::Detailed;
}

QString PlacemarkPositionProviderPlugin::name() const
{
    return tr( "Placemark position provider Plugin" );
}

QString PlacemarkPositionProviderPlugin::nameId() const
{
    return QStringLiteral("Placemark");
}

QString PlacemarkPositionProviderPlugin::guiString() const
{
    return tr( "Placemark" );
}

QString PlacemarkPositionProviderPlugin::version() const
{
    return QStringLiteral("1.0");
}

QString PlacemarkPositionProviderPlugin::description() const
{
    return tr( "Reports the position of a placemark" );
}

QString PlacemarkPositionProviderPlugin::copyrightYears() const
{
    return QStringLiteral("2011, 2012");
}

QVector<PluginAuthor> PlacemarkPositionProviderPlugin::pluginAuthors() const
{
    return QVector<PluginAuthor>()
            << PluginAuthor(QStringLiteral("Guillaume Martres"), QStringLiteral("smarter@ubuntu.com"))
            << PluginAuthor(QStringLiteral("Bernhard Beschow"), QStringLiteral("bbeschow@cs.tu-berlin.de"));
}

QIcon PlacemarkPositionProviderPlugin::icon() const
{
    return QIcon();
}

void PlacemarkPositionProviderPlugin::initialize()
{
    if ( m_marbleModel ) {
        setPlacemark( m_marbleModel->trackedPlacemark() );
        connect( m_marbleModel, SIGNAL(trackedPlacemarkChanged(const GeoDataPlacemark*)),
                 this, SLOT(setPlacemark(const GeoDataPlacemark*)) );
    } else {
        mDebug() << "PlacemarkPositionProviderPlugin: MarbleModel not set, cannot track placemarks.";
    }
    m_isInitialized = true;
}

bool PlacemarkPositionProviderPlugin::isInitialized() const
{
    return m_isInitialized;
}

PositionProviderPlugin* PlacemarkPositionProviderPlugin::newInstance() const
{
    return new PlacemarkPositionProviderPlugin( m_marbleModel );
}

PositionProviderStatus PlacemarkPositionProviderPlugin::status() const
{
    return m_status;
}

GeoDataCoordinates PlacemarkPositionProviderPlugin::position() const
{
    return m_coordinates;
}

GeoDataAccuracy PlacemarkPositionProviderPlugin::accuracy() const
{
    return m_accuracy;
}

qreal PlacemarkPositionProviderPlugin::speed() const
{
    return m_speed;
}

qreal PlacemarkPositionProviderPlugin::direction() const
{
    return m_direction;
}

QDateTime PlacemarkPositionProviderPlugin::timestamp() const
{
    return m_marbleModel->clockDateTime();
}

void PlacemarkPositionProviderPlugin::setPlacemark( const GeoDataPlacemark *placemark )
{
    const GeoDataPlacemark *const oldPlacemark = m_placemark;

    if ( oldPlacemark != 0 ) {
        emit statusChanged( PositionProviderStatusUnavailable );
    }

    m_placemark   = placemark;
    m_timestamp   = placemark ? m_marbleModel->clockDateTime() : QDateTime();
    GeoDataCoordinates const newCoordinates = placemark ? placemark->coordinate( m_timestamp ) : GeoDataCoordinates();
    if ( m_coordinates.isValid() && newCoordinates.isValid() ) {
        m_direction = m_coordinates.bearing( newCoordinates, GeoDataCoordinates::Degree, GeoDataCoordinates::FinalBearing );
    }
    m_coordinates = newCoordinates;
    m_status      = placemark ? PositionProviderStatusAvailable : PositionProviderStatusUnavailable;
    m_speed       = 0.0;

    disconnect( m_marbleModel->clock(), SIGNAL(timeChanged()), this, SLOT(updatePosition()) );
    if ( placemark ) {
        connect( m_marbleModel->clock(), SIGNAL(timeChanged()), this, SLOT(updatePosition()) );
    }

    if ( oldPlacemark != m_placemark && m_placemark != 0 ) {
        emit statusChanged( m_status );
    }

    if ( m_status == PositionProviderStatusAvailable ) {
        emit positionChanged( m_coordinates, m_accuracy );
    }
}

void PlacemarkPositionProviderPlugin::updatePosition()
{
    if ( m_placemark == 0 ) {
        return;
    }

    Q_ASSERT( m_marbleModel && "MarbleModel missing in PlacemarkPositionProviderPlugin" );

    const GeoDataCoordinates previousCoordinates = m_coordinates;
    m_coordinates = m_placemark->coordinate( m_marbleModel->clock()->dateTime() );
    m_direction = previousCoordinates.bearing( m_coordinates, GeoDataCoordinates::Degree, GeoDataCoordinates::FinalBearing );

    if ( m_timestamp.isValid() ) {
        const qreal averageAltitude = ( m_coordinates.altitude() + m_coordinates.altitude() ) / 2.0 + m_marbleModel->planetRadius();
        const qreal distance = distanceSphere( previousCoordinates, m_coordinates ) * averageAltitude;
        const qreal seconds = m_timestamp.msecsTo( m_marbleModel->clockDateTime() ) / 1000.0;
        m_speed = ( seconds > 0 ) ? ( distance / seconds ) : 0;
    }
    else {
        m_speed = 0;
    }

    m_timestamp = m_marbleModel->clockDateTime();

    emit positionChanged( m_coordinates, m_accuracy );
}

#include "moc_PlacemarkPositionProviderPlugin.cpp"
