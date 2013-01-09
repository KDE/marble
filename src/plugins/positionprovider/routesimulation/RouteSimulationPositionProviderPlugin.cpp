//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011      Konrad Enzensberger <e.konrad@mpegcode.com>
// Copyright 2011      Dennis Nienhüser <earthwings@gentoo.org>
// Copyright 2012      Bernhard Beschow <bbeschow@cs.tu-berlin.de>
//

#include "RouteSimulationPositionProviderPlugin.h"

#include <QtCore/QTimer>

#include "GeoDataCoordinates.h"
#include "GeoDataDocument.h"
#include "GeoDataFolder.h"
#include "GeoDataPlacemark.h"
#include "MarbleDebug.h"
#include "MarbleModel.h"
#include "routing/AlternativeRoutesModel.h"
#include "routing/RoutingManager.h"

namespace Marble
{

namespace {
    qreal const c_frequency = 4.0; // Hz
}

QString RouteSimulationPositionProviderPlugin::name() const
{
    return tr( "Current Route Position Provider Plugin" );
}

QString RouteSimulationPositionProviderPlugin::nameId() const
{
    return "RouteSimulationPositionProviderPlugin";
}

QString RouteSimulationPositionProviderPlugin::guiString() const
{
    return tr( "Current Route" );
}

QString RouteSimulationPositionProviderPlugin::version() const
{
    return "1.1";
}

QString RouteSimulationPositionProviderPlugin::description() const
{
    return tr( "Simulates travelling along the current route." );
}

QString RouteSimulationPositionProviderPlugin::copyrightYears() const
{
    return "2011, 2012";
}

QList<PluginAuthor> RouteSimulationPositionProviderPlugin::pluginAuthors() const
{
    return QList<PluginAuthor>()
            << PluginAuthor( "Konrad Enzensberger", "e.konrad@mpegcode.com" )
            << PluginAuthor( QString::fromUtf8( "Dennis Nienhüser" ), "earthwings@gentoo.org" )
            << PluginAuthor( "Bernhard Beschow", "bbeschow@cs.tu-berlin.de" );
}

QIcon RouteSimulationPositionProviderPlugin::icon() const
{
    return QIcon();
}

PositionProviderPlugin* RouteSimulationPositionProviderPlugin::newInstance() const
{
    return new RouteSimulationPositionProviderPlugin;
}

PositionProviderStatus RouteSimulationPositionProviderPlugin::status() const
{
    return m_status;
}

GeoDataCoordinates RouteSimulationPositionProviderPlugin::position() const
{
    return m_currentPosition;
}

GeoDataAccuracy RouteSimulationPositionProviderPlugin::accuracy() const
{
    GeoDataAccuracy result;

    // faked values
    result.level = GeoDataAccuracy::Detailed;
    result.horizontal = 10.0;
    result.vertical = 10.0;

    return result;
}

RouteSimulationPositionProviderPlugin::RouteSimulationPositionProviderPlugin() :
    m_currentIndex( -2 ),
    m_status( PositionProviderStatusUnavailable ),
    m_direction( 0.0 )
{
    // nothing to do
}

RouteSimulationPositionProviderPlugin::~RouteSimulationPositionProviderPlugin()
{
}

void RouteSimulationPositionProviderPlugin::initialize()
{
    m_currentIndex = -1;

    m_lineString.clear();

    GeoDataDocument* document = const_cast<MarbleModel *>( marbleModel() )->routingManager()->alternativeRoutesModel()->currentRoute();
    if ( document && document->size() > 0 ) {
        foreach( const GeoDataPlacemark *placemark, document->placemarkList() ) {
            GeoDataGeometry* geometry = placemark->geometry();
            GeoDataLineString* lineString = dynamic_cast<GeoDataLineString*>( geometry );
            if ( lineString ) {
                m_lineString << *lineString;
            }
        }
    }

    m_status = m_lineString.isEmpty() ? PositionProviderStatusUnavailable : PositionProviderStatusAcquiring;

    if ( !m_lineString.isEmpty() ) {
        QTimer::singleShot( 1000.0 / c_frequency, this, SLOT(update()) );
    }
}

bool RouteSimulationPositionProviderPlugin::isInitialized() const
{
    return ( m_currentIndex > -2 );
}

qreal RouteSimulationPositionProviderPlugin::speed() const
{
    /** @todo: calculate speed */
    return 0.0;
}

qreal RouteSimulationPositionProviderPlugin::direction() const
{
    return m_direction;
}

QDateTime RouteSimulationPositionProviderPlugin::timestamp() const
{
    return QDateTime::currentDateTime();
}

void RouteSimulationPositionProviderPlugin::update()
{
    ++m_currentIndex;

    if ( m_currentIndex >= 0 && m_currentIndex < m_lineString.size() ) {
        if ( m_status != PositionProviderStatusAvailable ) {
            m_status = PositionProviderStatusAvailable;
            emit statusChanged( PositionProviderStatusAvailable );
        }

        GeoDataCoordinates newPosition = m_lineString.at( m_currentIndex );
        if ( m_currentPosition.isValid() ) {
            m_direction = m_currentPosition.bearing( newPosition, GeoDataCoordinates::Degree, GeoDataCoordinates::FinalBearing );
        }
        m_currentPosition = newPosition;
        emit positionChanged( position(), accuracy() );
    }
    else {
        // Repeat from start
        m_currentIndex = -1;
        if ( m_status != PositionProviderStatusUnavailable ) {
            m_status = PositionProviderStatusUnavailable;
            emit statusChanged( PositionProviderStatusUnavailable );
        }
    }

    QTimer::singleShot( 1000.0 / c_frequency, this, SLOT(update()) );
}

} // namespace Marble

Q_EXPORT_PLUGIN2( RouteSimulationPositionProviderPlugin, Marble::RouteSimulationPositionProviderPlugin )

#include "RouteSimulationPositionProviderPlugin.moc"
