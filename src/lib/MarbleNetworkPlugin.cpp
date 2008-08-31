//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2008 Pino Toscano <pino@kde.org>
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


MarbleNetworkPlugin::MarbleNetworkPlugin()
    : d( new MarbleNetworkPluginPrivate() )
{
}

MarbleNetworkPlugin::~MarbleNetworkPlugin()
{
    delete d;
}

}

#include "MarbleNetworkPlugin.moc"

