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

#include "GosmoreReverseGeocodingPlugin.h"
#include "GosmoreReverseGeocodingRunner.h"
#include "MarbleDirs.h"

#include <QDir>
#include <QFileInfo>

namespace Marble
{

GosmorePlugin::GosmorePlugin( QObject *parent ) :
    ReverseGeocodingRunnerPlugin( parent )
{
    setSupportedCelestialBodies(QStringList(QStringLiteral("earth")));
    setCanWorkOffline( true );
}

QString GosmorePlugin::name() const
{
    return tr( "Gosmore Reverse Geocoding" );
}

QString GosmorePlugin::guiString() const
{
    return tr( "Gosmore" );
}

QString GosmorePlugin::nameId() const
{
    return QStringLiteral("gosmore-reverse");
}

QString GosmorePlugin::version() const
{
    return QStringLiteral("1.0");
}

QString GosmorePlugin::description() const
{
    return tr( "Offline reverse geocoding using Gosmore." );
}

QString GosmorePlugin::copyrightYears() const
{
    return QStringLiteral("2010, 2012");
}

QVector<PluginAuthor> GosmorePlugin::pluginAuthors() const
{
    return QVector<PluginAuthor>()
            << PluginAuthor(QStringLiteral("Dennis Nienhüser"), QStringLiteral("nienhueser@kde.org"))
            << PluginAuthor(QStringLiteral("Bernhard Beschow"), QStringLiteral("bbeschow@cs.tu-berlin.de"));
}

ReverseGeocodingRunner* GosmorePlugin::newRunner() const
{
    return new GosmoreRunner;
}

bool GosmorePlugin::canWork() const
{
    QDir mapDir(MarbleDirs::localPath() + QLatin1String("/maps/earth/gosmore/"));
    QFileInfo mapFile = QFileInfo ( mapDir, "gosmore.pak" );
    return mapFile.exists();
}

}

#include "moc_GosmoreReverseGeocodingPlugin.cpp"
