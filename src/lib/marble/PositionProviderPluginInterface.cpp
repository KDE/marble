// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2009 Eckhart Wörner <ewoerner@kde.org>
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

    if (status() == PositionProviderStatusError) {
        return QObject::tr("Unknown error");
    }

    return {};
}

} // namespace Marble
