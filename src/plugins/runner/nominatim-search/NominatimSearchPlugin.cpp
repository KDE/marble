// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2010 Dennis Nienhüser <nienhueser@kde.org>
// SPDX-FileCopyrightText: 2012 Bernhard Beschow <bbeschow@cs.tu-berlin.de>
//

#include "NominatimSearchPlugin.h"
#include "OsmNominatimSearchRunner.h"

namespace Marble
{

NominatimPlugin::NominatimPlugin( QObject *parent ) :
    SearchRunnerPlugin( parent )
{
    setSupportedCelestialBodies(QStringList(QStringLiteral("earth")));
    setCanWorkOffline( false );
}

QString NominatimPlugin::name() const
{
    return tr( "OpenStreetMap Nominatim Search" );
}

QString NominatimPlugin::guiString() const
{
    return tr( "OpenStreetMap Nominatim" );
}

QString NominatimPlugin::nameId() const
{
    return QStringLiteral("nominatim-search");
}

QString NominatimPlugin::version() const
{
    return QStringLiteral("1.0");
}

QString NominatimPlugin::description() const
{
    return tr( "Online search for placemarks using the OpenStreetMap Nominatim service" );
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

SearchRunner* NominatimPlugin::newRunner() const
{
    return new OsmNominatimRunner;
}

}

#include "moc_NominatimSearchPlugin.cpp"
