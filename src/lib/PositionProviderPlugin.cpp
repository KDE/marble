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
        : m_marbleModel( 0 )
    {
    }

    const MarbleModel *m_marbleModel;
};

PositionProviderPlugin::PositionProviderPlugin()
    : d( new PositionProviderPluginPrivate() )
{
}

PositionProviderPlugin::~PositionProviderPlugin()
{
}

const MarbleModel* PositionProviderPlugin::marbleModel() const
{
    return d->m_marbleModel;
}

void PositionProviderPlugin::setMarbleModel( const MarbleModel* marbleModel )
{
    d->m_marbleModel = marbleModel;
}

qreal PositionProviderPlugin::speed() const
{
    return 0;
}

qreal PositionProviderPlugin::direction() const
{
    return 0;
}

}

#include "PositionProviderPlugin.moc"
