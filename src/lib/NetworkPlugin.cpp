//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2008 Pino Toscano <pino@kde.org>
// Copyright 2008 Jens-Michael Hoffmann <jensmh@gmx.de>
//


#include "NetworkPlugin.h"

namespace Marble
{

class NetworkPluginPrivate
{
  public:
    NetworkPluginPrivate()
    {
    }

    ~NetworkPluginPrivate()
    {
    }

};

// Do not create the NetworkPluginPrivate, because it holds
// no data at the moment (may change in the future).
NetworkPlugin::NetworkPlugin()
    : d( 0 )
{
}

NetworkPlugin::~NetworkPlugin()
{
    //delete d; //for the future :)
}

}

#include "NetworkPlugin.moc"
