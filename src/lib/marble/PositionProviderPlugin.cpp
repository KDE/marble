//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009 Eckhart Wörner <ewoerner@kde.org>
//

#include "PositionProviderPlugin.h"

namespace Marble
{

class PositionProviderPluginPrivate
{
public:
    PositionProviderPluginPrivate()
    {
    }
};

PositionProviderPlugin::PositionProviderPlugin()
    : d( new PositionProviderPluginPrivate() )
{
}

PositionProviderPlugin::~PositionProviderPlugin()
{
    delete d;
}

}

#include "moc_PositionProviderPlugin.cpp"
