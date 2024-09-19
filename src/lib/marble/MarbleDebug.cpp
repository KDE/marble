// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2009 Patrick Spendrin <ps_ml@gmx.de>
//

#include "MarbleDebug.h"
#include <QIODevice>

/**
 * All of the logging categories supported by Marble.
 *
 * Add any new logging categories as required here.
 * See @c MarbleDebug.h for more information.
 */
MARBLE_EXPORT Q_LOGGING_CATEGORY(MARBLE_DEFAULT, "marble_default") MARBLE_EXPORT Q_LOGGING_CATEGORY(MARBLE_LIB, "marble_lib") MARBLE_EXPORT
    Q_LOGGING_CATEGORY(MARBLE_PLUGINS, "marble_plugins")

        namespace Marble
{
    bool MarbleDebug::m_enabled = false;

    bool MarbleDebug::isEnabled()
    {
        return MarbleDebug::m_enabled;
    }

    void MarbleDebug::setEnabled(bool enabled)
    {
        qCWarning(MARBLE_DEFAULT) << "Control of debug output using this function is no longer implemented, use the Qt logging settings";
        MarbleDebug::m_enabled = enabled;
    }

} // namespace Marble
