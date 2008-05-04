//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2008 Torsten Rahn <tackat@kde.org>"
//

#include "MarbleTestPlugin.h"


QStringList MarbleTestPlugin::backendTypes() const
{
    return QStringList( "test" );
}

QString MarbleTestPlugin::renderPolicy() const
{
    return QString( "ALWAYS" );
}

QString MarbleTestPlugin::renderPosition() const
{
    return QString( "ALWAYS_ON_TOP" );
}

QString MarbleTestPlugin::name() const
{
    return QString( "Test Plugin" );
}

QString MarbleTestPlugin::nameId() const
{
    return QString( "test-plugin" );
}

QString MarbleTestPlugin::description() const
{
    return QString( "This is a simple test plugin." );
}

QIcon MarbleTestPlugin::icon () const
{
    return QIcon();
}


void MarbleTestPlugin::initialize ()
{
}

bool MarbleTestPlugin::isInitialized () const
{
    return true;
}

bool MarbleTestPlugin::render( ClipPainter *painter, GeoSceneLayer * layer, ViewportParams *viewport )
{
    return true;
}

Q_EXPORT_PLUGIN2(MarbleTestPlugin, MarbleTestPlugin)

#include "MarbleTestPlugin.moc"
