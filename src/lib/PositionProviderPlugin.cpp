//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009 Eckhart Wörner <ewoerner@kde.org>
//

#include "PositionProviderPlugin.h"
using namespace Marble;


Marble::PositionProviderPlugin::PositionProviderPlugin()
{
}

Marble::PositionProviderPlugin::~PositionProviderPlugin()
{
}

qreal Marble::PositionProviderPlugin::speed() const
{
    return 0;
}

qreal Marble::PositionProviderPlugin::direction() const
{
    return 0;
}


#include "PositionProviderPlugin.moc"
