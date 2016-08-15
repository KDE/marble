//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010      Dennis Nienhüser <nienhueser@kde.org>
// Copyright 2012      Bernhard Beschow <bbeschow@cs.tu-berlin.de>
//

#include "NominatimReverseGeocodingPlugin.h"
#include "OsmNominatimReverseGeocodingRunner.h"

namespace Marble
{

NominatimPlugin::NominatimPlugin( QObject *parent ) :
    ReverseGeocodingRunnerPlugin( parent )
{
    setSupportedCelestialBodies(QStringList(QStringLiteral("earth")));
    setCanWorkOffline( false );
}

QString NominatimPlugin::name() const
{
    return tr( "OpenStreetMap Nominatim Reverse Geocoding" );
}

QString NominatimPlugin::guiString() const
{
    return tr( "OpenStreetMap Nominatim" );
}

QString NominatimPlugin::nameId() const
{
    return QStringLiteral("nominatim-reverse");
}

QString NominatimPlugin::version() const
{
    return QStringLiteral("1.0");
}

QString NominatimPlugin::description() const
{
    return tr( "Online reverse geocoding using the OpenStreetMap Nominatim service" );
}

QString NominatimPlugin::copyrightYears() const
{
    return QStringLiteral("2010, 2012");
}

QVector<PluginAuthor> NominatimPlugin::pluginAuthors() const
{
    return QVector<PluginAuthor>()
            << PluginAuthor(QStringLiteral("Dennis Nienhüser"), QStringLiteral("nienhueser@kde.org"))
            << PluginAuthor(QStringLiteral("Bernhard Beschow"), QStringLiteral("bbeschow@cs.tu-berlin.de"));
}

ReverseGeocodingRunner* NominatimPlugin::newRunner() const
{
    return new OsmNominatimRunner;
}

}

#include "moc_NominatimReverseGeocodingPlugin.cpp"
