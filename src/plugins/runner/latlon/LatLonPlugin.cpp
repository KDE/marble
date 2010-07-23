//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010      Dennis Nienhüser <earthwings@gentoo.org>
//

#include "LatLonPlugin.h"
#include "LatLonRunner.h"

namespace Marble
{

LatLonPlugin::LatLonPlugin( QObject *parent ) : RunnerPlugin( parent )
{
    setCapabilities( Search );
    setName( "Geographic Coordinates" );
    setNameId( "latlon" );
    setDescription( "Direct input of geographic coordinates" );
    setGuiString( "Geographic Coordinates" );
}

MarbleAbstractRunner* LatLonPlugin::newRunner() const
{
    return new LatLonRunner;
}

}

Q_EXPORT_PLUGIN2( LatLonPlugin, Marble::LatLonPlugin )

#include "LatLonPlugin.moc"
