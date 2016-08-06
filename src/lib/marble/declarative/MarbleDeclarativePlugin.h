//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010      Dennis Nienhüser <nienhueser@kde.org>
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
    virtual void registerTypes( const char *uri );

    void initializeEngine( QQmlEngine *engine, const char *);
};

#endif
