//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009 Eckhart Wörner <ewoerner@kde.org>
//

#include "PositionProviderPluginInterface.h"

namespace Marble
{

PositionProviderPluginInterface::~PositionProviderPluginInterface()
{
    // nothing to do
}

QString PositionProviderPluginInterface::error() const
{
    // Subclasses are expected to override this, but we provide
    // a default implementation

    if ( status() == PositionProviderStatusError )
    {
        return QObject::tr( "Unknown error" );
    }

    return QString();
}

} // namespace Marble
