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
#include "MarbleModel.h"
#include "MarbleDebug.h"

using namespace Marble;

PlacemarkPositionProviderPlugin::PlacemarkPositionProviderPlugin()
    : PositionProviderPlugin(),
      m_placemark( 0 ),
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

QString PlacemarkPositionProviderPlugin::description() const
{
    return tr( "Reports the position of a placemark" );
}

QIcon PlacemarkPositionProviderPlugin::icon() const
{
    return QIcon();
}

void PlacemarkPositionProviderPlugin::initialize()
{
    if ( marbleModel() ) {
        setPlacemark( marbleModel()->trackedPlacemark() );
    } else {
        mDebug() << "PlacemarkPositionProviderPlugin: MarbleModel not set, cannot track placemarks.";
    }
    m_isInitialized = true;
}

bool PlacemarkPositionProviderPlugin::isInitialized() const
{
    return true;
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
    if ( m_placemark == 0 ) {
        return GeoDataCoordinates();
    }
    return m_placemark->coordinate();
}

GeoDataAccuracy PlacemarkPositionProviderPlugin::accuracy() const
{
    return m_accuracy;
}

void PlacemarkPositionProviderPlugin::setPlacemark( const GeoDataPlacemark *placemark )
{
    disconnect( marbleModel()->clock(), SIGNAL( timeChanged() ), this, SLOT( updatePosition() ) );

    m_placemark = placemark;
    if ( placemark ) {
        connect( marbleModel()->clock(), SIGNAL( timeChanged() ), this, SLOT( updatePosition() ) );
    }

    update();
}

void PlacemarkPositionProviderPlugin::updatePosition()
{
    if ( m_placemark == 0 ) {
        return;
    }

    Q_ASSERT( marbleModel() && "MarbleModel missing in PlacemarkPositionProviderPlugin" );
    emit positionChanged(m_placemark->coordinate( marbleModel()->clock()->dateTime() ), m_accuracy);
}

void PlacemarkPositionProviderPlugin::update()
{
    if ( m_placemark != 0 ) {
        m_status = PositionProviderStatusAvailable;
    } else {
        m_status = PositionProviderStatusUnavailable;
    }
    emit statusChanged( m_status );
}

Q_EXPORT_PLUGIN2( PlacemarkPositionProviderPlugin, Marble::PlacemarkPositionProviderPlugin )

#include "PlacemarkPositionProviderPlugin.moc"
