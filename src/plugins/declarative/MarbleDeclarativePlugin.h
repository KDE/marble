//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010      Dennis Nienhüser <earthwings@gentoo.org>
//

#ifndef MARBLE_DECLARATIVE_PLUGIN_H
#define MARBLE_DECLARATIVE_PLUGIN_H

#include <QtDeclarative/QDeclarativeExtensionPlugin>

namespace Marble
{
namespace Declarative
{

/**
  * Registers MarbleWidget, MarbleRunnerManager and MarbleThemeManager
  * as QDeclarative extensions for use in QML.
  */
class MarbleDeclarativePlugin : public QDeclarativeExtensionPlugin
{
    Q_OBJECT
public:
    /** Overriding QDeclarativeExtensionPlugin to register types */
    virtual void registerTypes( const char *uri );

    /** Overriding QDeclarativeExtensionPlugin to register image provider */
    void initializeEngine( QDeclarativeEngine *engine, const char *);
};

} // namespace Declarative
} // namespace Marble

#endif
