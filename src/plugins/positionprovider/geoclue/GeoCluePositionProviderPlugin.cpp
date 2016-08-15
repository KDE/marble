//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009      Eckhart Wörner <ewoerner@kde.org>
//

#include "GeoCluePositionProviderPlugin.h"

#include "GeoCute/PositionProvider.h"



using namespace Marble;

QString GeoCluePositionProviderPlugin::name() const
{
    return tr( "GeoClue position provider Plugin" );
}

QString GeoCluePositionProviderPlugin::nameId() const
{
    return QStringLiteral("GeoClue");
}

QString GeoCluePositionProviderPlugin::guiString() const
{
    return tr( "GeoClue" );
}

QString GeoCluePositionProviderPlugin::description() const
{
    return tr( "Reports the position via the GeoClue Location Framework." );
}

QIcon GeoCluePositionProviderPlugin::icon() const
{
    return QIcon();
}

void GeoCluePositionProviderPlugin::initialize()
{
    m_positionProvider = GeoCute::PositionProvider::detailed();
    if (m_positionProvider) {
        connect( m_positionProvider, SIGNAL(positionChanged(GeoCute::Position)),
                 SLOT(updatePosition(GeoCute::Position)) );
        connect( m_positionProvider, SIGNAL(statusChanged(GeoCute::Status)),
                 SLOT(updateStatus(GeoCute::Status)) );
    }
}

bool GeoCluePositionProviderPlugin::isInitialized() const
{
    return m_positionProvider;
}

PositionProviderPlugin* GeoCluePositionProviderPlugin::newInstance() const
{
    return new GeoCluePositionProviderPlugin;
}

PositionProviderStatus GeoCluePositionProviderPlugin::status() const
{
    return m_status;
}

GeoDataCoordinates GeoCluePositionProviderPlugin::position() const
{
    return m_position;
}

GeoDataAccuracy GeoCluePositionProviderPlugin::accuracy() const
{
    return m_accuracy;
}

GeoCluePositionProviderPlugin::GeoCluePositionProviderPlugin() : m_positionProvider(0)
{
}

GeoCluePositionProviderPlugin::~GeoCluePositionProviderPlugin()
{
    delete m_positionProvider;
}

void GeoCluePositionProviderPlugin::updatePosition(GeoCute::Position newPosition)
{
    m_position = GeoDataCoordinates(newPosition.longitude, newPosition.latitude,
                                    newPosition.altitude, GeoDataCoordinates::Degree);
    m_accuracy.level = static_cast<GeoDataAccuracy::Level>(newPosition.accuracy.level);
    m_accuracy.horizontal = newPosition.accuracy.horizontal;
    m_accuracy.vertical = newPosition.accuracy.vertical;

    emit positionChanged(m_position, m_accuracy);
}

void GeoCluePositionProviderPlugin::updateStatus(GeoCute::Status newStatus)
{
    m_status = static_cast<PositionProviderStatus>(newStatus);

    emit statusChanged(m_status);
}

#include "moc_GeoCluePositionProviderPlugin.cpp"
