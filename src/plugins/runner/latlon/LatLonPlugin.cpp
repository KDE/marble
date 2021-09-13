// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2010 Dennis Nienhüser <nienhueser@kde.org>
//

#include "LatLonPlugin.h"
#include "LatLonRunner.h"

namespace Marble
{

LatLonPlugin::LatLonPlugin( QObject *parent ) :
    SearchRunnerPlugin( parent )
{
}

QString LatLonPlugin::name() const
{
    return tr( "Geographic Coordinates Search" );
}

QString LatLonPlugin::guiString() const
{
    return tr( "Geographic Coordinates" );
}

QString LatLonPlugin::nameId() const
{
    return QStringLiteral("latlon");
}

QString LatLonPlugin::version() const
{
    return QStringLiteral("1.0");
}

QString LatLonPlugin::description() const
{
    return tr( "Direct input of geographic coordinates" );
}

QString LatLonPlugin::copyrightYears() const
{
    return QStringLiteral("2010");
}

QVector<PluginAuthor> LatLonPlugin::pluginAuthors() const
{
    return QVector<PluginAuthor>()
            << PluginAuthor(QStringLiteral("Dennis Nienhüser"), QStringLiteral("nienhueser@kde.org"));
}

SearchRunner* LatLonPlugin::newRunner() const
{
    return new LatLonRunner;
}

}

#include "moc_LatLonPlugin.cpp"
