//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2016  Friedrich W. H. Kossebau <kossebau@kde.org>
//

#include "GeoUriPlugin.h"

#include "GeoUriRunner.h"

namespace Marble
{

GeoUriPlugin::GeoUriPlugin(QObject *parent)
    : SearchRunnerPlugin(parent)
{
}

QString GeoUriPlugin::name() const
{
    return tr("Geo URI Search");
}

QString GeoUriPlugin::guiString() const
{
    return tr("Geo URI");
}

QString GeoUriPlugin::nameId() const
{
    return QStringLiteral("geouri");
}

QString GeoUriPlugin::version() const
{
    return QStringLiteral("1.0");
}

QString GeoUriPlugin::description() const
{
    return tr("Input of geographic coordinates by the geo URI scheme");
}

QString GeoUriPlugin::copyrightYears() const
{
    return QStringLiteral("2016");
}

QVector<PluginAuthor> GeoUriPlugin::pluginAuthors() const
{
    return QVector<PluginAuthor>()
            << PluginAuthor(QStringLiteral("Friedrich W. H. Kossebau"), QStringLiteral("kossebau@kde.org"));
}

SearchRunner* GeoUriPlugin::newRunner() const
{
    return new GeoUriRunner;
}

}

#include "moc_GeoUriPlugin.cpp"
