//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009      Eckhart Wörner <ewoerner@kde.org>
// Copyright 2011      Bernhard Beschow <bbeschow@cs.tu-berlin.de>
//

#include "PlacemarkPositionProviderPlugin.h"

#include "GeoDataPlacemark.h"
#include "MarbleClock.h"
#include "MarbleMath.h"
#include "MarbleModel.h"
#include "MarbleDebug.h"

using namespace Marble;

PlacemarkPositionProviderPlugin::PlacemarkPositionProviderPlugin()
    : PositionProviderPlugin(),
      m_placemark( 0 ),
      m_speed( 0 ),
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
    return QString::fromLatin1( "Placemark" );
}

QString PlacemarkPositionProviderPlugin::guiString() const
{
    return tr( "Placemark" );
}

QString PlacemarkPositionProviderPlugin::version() const
{
    return "1.0";
}

QString PlacemarkPositionProviderPlugin::description() const
{
    return tr( "Reports the position of a placemark" );
}

QString PlacemarkPositionProviderPlugin::copyrightYears() const
{
    return "2009, 2011";
}

QList<PluginAuthor> PlacemarkPositionProviderPlugin::pluginAuthors() const
{
    return QList<PluginAuthor>()
            << PluginAuthor( QString::fromUtf8( "Eckhart Wörner" ), "ewoerner@kde.org" )
            << PluginAuthor( "Bernhard Beschow", "bbeschow@cs.tu-berlin.de" );
}

QIcon PlacemarkPositionProviderPlugin::icon() const
{
    return QIcon();
}

void PlacemarkPositionProviderPlugin::initialize()
{
    if ( marbleModel() ) {
        setPlacemark( marbleModel()->trackedPlacemark() );
        connect( marbleModel(), SIGNAL( trackedPlacemarkChanged( const GeoDataPlacemark* ) ),
                 this, SLOT( setPlacemark( const GeoDataPlacemark * ) ) );
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
    return new PlacemarkPositionProviderPlugin();
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
    return 0.0;
}

QDateTime PlacemarkPositionProviderPlugin::timestamp() const
{
    return marbleModel()->clockDateTime();
}

void PlacemarkPositionProviderPlugin::setPlacemark( const GeoDataPlacemark *placemark )
{
    const GeoDataPlacemark *const oldPlacemark = m_placemark;

    if ( oldPlacemark != 0 ) {
        emit statusChanged( PositionProviderStatusUnavailable );
    }

    m_placemark   = placemark;
    m_timestamp   = placemark ? marbleModel()->clockDateTime() : QDateTime();
    m_coordinates = placemark ? placemark->coordinate( m_timestamp ) : GeoDataCoordinates();
    m_status      = placemark ? PositionProviderStatusAvailable : PositionProviderStatusUnavailable;
    m_speed       = 0.0;

    disconnect( marbleModel()->clock(), SIGNAL( timeChanged() ), this, SLOT( updatePosition() ) );
    if ( placemark ) {
        connect( marbleModel()->clock(), SIGNAL( timeChanged() ), this, SLOT( updatePosition() ) );
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

    Q_ASSERT( marbleModel() && "MarbleModel missing in PlacemarkPositionProviderPlugin" );

    const GeoDataCoordinates previousCoordinates = m_coordinates;
    m_coordinates = m_placemark->coordinate( marbleModel()->clock()->dateTime() );

    if ( m_timestamp.isValid() ) {
        const qreal averageAltitude = ( m_coordinates.altitude() + m_coordinates.altitude() ) / 2.0 + marbleModel()->planetRadius();
        const qreal distance = distanceSphere( previousCoordinates, m_coordinates ) * averageAltitude;
#if QT_VERSION >= 0x40700
        const qreal seconds = m_timestamp.msecsTo( marbleModel()->clockDateTime() ) / 1000.0;
#else
        const qreal seconds = m_timestamp.secsTo( marbleModel()->clockDateTime() );
#endif
        m_speed = ( seconds > 0 ) ? ( distance / seconds ) : 0;
    }
    else {
        m_speed = 0;
    }

    m_timestamp = marbleModel()->clockDateTime();

    emit positionChanged( m_coordinates, m_accuracy );
}

Q_EXPORT_PLUGIN2( PlacemarkPositionProviderPlugin, Marble::PlacemarkPositionProviderPlugin )

#include "PlacemarkPositionProviderPlugin.moc"
