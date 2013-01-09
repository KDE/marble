//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010      Dennis Nienhüser <earthwings@gentoo.org>
// Copyright 2012      Bernhard Beschow <bbeschow@cs.tu-berlin.de>
//

#include "GosmoreReverseGeocodingPlugin.h"
#include "GosmoreReverseGeocodingRunner.h"
#include "MarbleDirs.h"

#include <QtCore/QDir>
#include <QtCore/QFileInfo>

namespace Marble
{

GosmorePlugin::GosmorePlugin( QObject *parent ) :
    ReverseGeocodingRunnerPlugin( parent )
{
    setSupportedCelestialBodies( QStringList() << "earth" );
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
    return "gosmore-reverse";
}

QString GosmorePlugin::version() const
{
    return "1.0";
}

QString GosmorePlugin::description() const
{
    return tr( "Offline reverse geocoding using Gosmore." );
}

QString GosmorePlugin::copyrightYears() const
{
    return "2010, 2012";
}

QList<PluginAuthor> GosmorePlugin::pluginAuthors() const
{
    return QList<PluginAuthor>()
            << PluginAuthor( QString::fromUtf8( "Dennis Nienhüser" ), "earthwings@gentoo.org" )
            << PluginAuthor( "Bernhard Beschow", "bbeschow@cs.tu-berlin.de" );
}

ReverseGeocodingRunner* GosmorePlugin::newRunner() const
{
    return new GosmoreRunner;
}

bool GosmorePlugin::canWork() const
{
    QDir mapDir( MarbleDirs::localPath() + "/maps/earth/gosmore/" );
    QFileInfo mapFile = QFileInfo ( mapDir, "gosmore.pak" );
    return mapFile.exists();
}

}

Q_EXPORT_PLUGIN2( GosmoreReverseGeocodingPlugin, Marble::GosmorePlugin )

#include "GosmoreReverseGeocodingPlugin.moc"
