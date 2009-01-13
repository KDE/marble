//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2008 Pino Toscano <pino@kde.org>
// Copyright 2008 Jens-Michael Hoffmann <jensmh@gmx.de>
//


#include "MarbleNetworkPlugin.h"

namespace Marble
{

class MarbleNetworkPluginPrivate
{
  public:
    MarbleNetworkPluginPrivate()
    {
    }

    ~MarbleNetworkPluginPrivate()
    {
    }

};

// Do not create the MarbleNetworkPluginPrivate, because it holds
// no data at the moment (may change in the future).
MarbleNetworkPlugin::MarbleNetworkPlugin()
    : d( 0 )
{
}

MarbleNetworkPlugin::~MarbleNetworkPlugin()
{
    //delete d; //for the future :)
}

}

#include "MarbleNetworkPlugin.moc"
