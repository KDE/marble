// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2010 Dennis Nienhüser <nienhueser@kde.org>
//

#ifndef MARBLE_DECLARATIVE_PLUGIN_H
#define MARBLE_DECLARATIVE_PLUGIN_H

#include "marble_declarative_export.h"

#include <QQmlExtensionPlugin>

/**
  * Registers MarbleQuickItem, MarbleRunnerManager and MarbleThemeManager
  * as QQml extensions for use in QML.
  */
class MARBLE_DECLARATIVE_EXPORT MarbleDeclarativePlugin : public QQmlExtensionPlugin
{
    // Disabled for now while libmarbledeclarative is used as shared library
    // Q_PLUGIN_METADATA(IID "org.kde.marble.MarbleDeclarativePlugin")
    Q_OBJECT
public:
    /** Overriding QQmlExtensionPlugin to register types */
    void registerTypes( const char *uri ) override;

    void initializeEngine( QQmlEngine *engine, const char *) override;
};

#endif
