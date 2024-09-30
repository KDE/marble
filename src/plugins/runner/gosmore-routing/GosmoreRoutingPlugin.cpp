// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2010 Dennis Nienhüser <nienhueser@kde.org>
// SPDX-FileCopyrightText: 2012 Bernhard Beschow <bbeschow@cs.tu-berlin.de>
//

#include "GosmoreRoutingPlugin.h"
#include "GosmoreRoutingRunner.h"
#include "MarbleDirs.h"

#include <QDir>
#include <QFileInfo>

namespace Marble
{

GosmorePlugin::GosmorePlugin(QObject *parent)
    : RoutingRunnerPlugin(parent)
{
    setSupportedCelestialBodies(QStringList(QStringLiteral("earth")));
    setCanWorkOffline(true);
}

QString GosmorePlugin::name() const
{
    return tr("Gosmore Routing");
}

QString GosmorePlugin::guiString() const
{
    return tr("Gosmore");
}

QString GosmorePlugin::nameId() const
{
    return QStringLiteral("gosmore-routing");
}

QString GosmorePlugin::version() const
{
    return QStringLiteral("1.0");
}

QString GosmorePlugin::description() const
{
    return tr("Offline route retrieval using Gosmore");
}

QString GosmorePlugin::copyrightYears() const
{
    return QStringLiteral("2010, 2012");
}

QVector<PluginAuthor> GosmorePlugin::pluginAuthors() const
{
    return QVector<PluginAuthor>() << PluginAuthor(QStringLiteral("Dennis Nienhüser"), QStringLiteral("nienhueser@kde.org"))
                                   << PluginAuthor(QStringLiteral("Bernhard Beschow"), QStringLiteral("bbeschow@cs.tu-berlin.de"));
}

RoutingRunner *GosmorePlugin::newRunner() const
{
    return new GosmoreRunner;
}

bool GosmorePlugin::canWork() const
{
    QDir mapDir(MarbleDirs::localPath() + QLatin1StringView("/maps/earth/gosmore/"));
    QFileInfo mapFile = QFileInfo(mapDir, "gosmore.pak");
    return mapFile.exists();
}

bool GosmorePlugin::supportsTemplate(RoutingProfilesModel::ProfileTemplate profileTemplate) const
{
    return profileTemplate == RoutingProfilesModel::CarFastestTemplate;
}

}

#include "moc_GosmoreRoutingPlugin.cpp"
