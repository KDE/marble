/*
    SPDX-License-Identifier: LGPL-2.1-or-later

    SPDX-FileCopyrightText: 2013 Ander Pijoan <ander.pijoan@deusto.es>
    SPDX-FileCopyrightText: 2019 John Zaitseff <J.Zaitseff@zap.org.au>
*/

#include "JsonPlugin.h"
#include "JsonRunner.h"

namespace Marble
{

JsonPlugin::JsonPlugin(QObject *parent)
    : ParseRunnerPlugin(parent)
{
}

QString JsonPlugin::name() const
{
    return tr("GeoJSON File Parser");
}

QString JsonPlugin::nameId() const
{
    return QStringLiteral("GeoJSON");
}

QString JsonPlugin::version() const
{
    return QStringLiteral("2.0");
}

QString JsonPlugin::description() const
{
    return tr("Create GeoDataDocument from GeoJSON Files");
}

QString JsonPlugin::copyrightYears() const
{
    return QStringLiteral("2012, 2019");
}

QVector<PluginAuthor> JsonPlugin::pluginAuthors() const
{
    return QVector<PluginAuthor>() << PluginAuthor(QStringLiteral("Ander Pijoan"), QStringLiteral("ander.pijoan@deusto.es"))
                                   << PluginAuthor(QStringLiteral("John Zaitseff"), QStringLiteral("J.Zaitseff@zap.org.au"));
}

QString JsonPlugin::fileFormatDescription() const
{
    return tr("GeoJSON");
}

QStringList JsonPlugin::fileExtensions() const
{
    return QStringList() << QStringLiteral("json") << QStringLiteral("geojson");
}

ParsingRunner *JsonPlugin::newRunner() const
{
    return new JsonRunner;
}

}

#include "moc_JsonPlugin.cpp"
