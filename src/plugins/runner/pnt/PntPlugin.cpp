//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011 Thibaut Gridel <tgridel@free.fr>

#include "PntPlugin.h"
#include "PntRunner.h"

namespace Marble
{

PntPlugin::PntPlugin( QObject *parent ) : RunnerPlugin( parent )
{
    setCapabilities( Parsing );
    setName( tr( "Pnt File Parser" ) );
    setNameId( "Pnt" );
    setDescription( tr( "Create GeoDataDocument from Pnt Files" ) );
    setGuiString( tr( "Pnt Parser" ) );
}

MarbleAbstractRunner* PntPlugin::newRunner() const
{
    return new PntRunner;
}

}

Q_EXPORT_PLUGIN2( PntPlugin, Marble::PntPlugin )

#include "PntPlugin.moc"
