// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2011 Thibaut Gridel <tgridel@free.fr>

#include "PntPlugin.h"
#include "PntRunner.h"

namespace Marble
{

PntPlugin::PntPlugin(QObject *parent)
    : ParseRunnerPlugin(parent)
{
}

QString PntPlugin::name() const
{
    return tr("Pnt File Parser");
}

QString PntPlugin::nameId() const
{
    return QStringLiteral("Pnt");
}

QString PntPlugin::version() const
{
    return QStringLiteral("1.0");
}

QString PntPlugin::description() const
{
    return tr("Create GeoDataDocument from Pnt Files");
}

QString PntPlugin::copyrightYears() const
{
    return QStringLiteral("2011");
}

QVector<PluginAuthor> PntPlugin::pluginAuthors() const
{
    return QVector<PluginAuthor>() << PluginAuthor(QStringLiteral("Thibaut Gridel"), QStringLiteral("tgridel@free.fr"));
}

QString PntPlugin::fileFormatDescription() const
{
    return tr("Micro World Database II");
}

QStringList PntPlugin::fileExtensions() const
{
    return QStringList(QStringLiteral("pnt"));
}

ParsingRunner *PntPlugin::newRunner() const
{
    return new PntRunner;
}

}

#include "moc_PntPlugin.cpp"
