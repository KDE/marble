// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2009 Eckhart Wörner <ewoerner@kde.org>
//

#include "PositionProviderPlugin.h"

namespace Marble
{

class PositionProviderPluginPrivate
{
public:
    PositionProviderPluginPrivate() = default;
};

PositionProviderPlugin::PositionProviderPlugin(QObject *parent)
    : QObject(parent)
    , d(new PositionProviderPluginPrivate())
{
}

PositionProviderPlugin::~PositionProviderPlugin()
{
    delete d;
}

}

#include "moc_PositionProviderPlugin.cpp"
