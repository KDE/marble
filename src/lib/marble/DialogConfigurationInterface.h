// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2012 Bernhard Beschow <bbeschow@cs.tu-berlin.de>
//
#ifndef MARBLE_DIALOGCONFIGURATIONINTERFACE_H
#define MARBLE_DIALOGCONFIGURATIONINTERFACE_H

#include <QObject> // for Q_DECLARE_INTERFACE macro
#include "marble_export.h"

class QDialog;

namespace Marble
{

/**
 * @brief This interface allows a plugin to provide a QWidget-based configuration
 * dialog which is accessible within Marble's own configuration dialog.
 *
 * @note In order for your plugin to provide a configuration dialog, derive your
 * plugin from this interface in addition to any other interfaces and classes.
 *
 * @note Make sure to deploy the @code Q_INTERFACES @endcode macro, which tells
 * Marble that you actually implemented the interface.
 */
class MARBLE_EXPORT DialogConfigurationInterface
{
 public:
    virtual ~DialogConfigurationInterface();

    /**
     * @brief Returns a pointer to the configuration dialog of the plugin.
     *
     * @return: Pointer to the configuration dialog, which must be non-zero.
     */
    virtual QDialog *configDialog() = 0;
};

}

Q_DECLARE_INTERFACE( Marble::DialogConfigurationInterface, "org.kde.Marble.DialogConfigurationInterface/1.0" )

#endif
