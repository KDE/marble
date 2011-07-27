//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011      Daniel Marth <danielmarth@gmx.at>
//

#ifndef MARBLE_QTCOMPONENTS_PLUGIN_H
#define MARBLE_QTCOMPONENTS_PLUGIN_H

#include <QtDeclarative/QDeclarativeExtensionPlugin>

namespace Marble
{
namespace Declarative
{

class QtComponentsPlugin : public QDeclarativeExtensionPlugin
{
    Q_OBJECT
public:
    /** Overriding QDeclarativeExtensionPlugin to register types */
    virtual void registerTypes( const char *uri );
};

} // namespace Declarative
} // namespace Marble

#endif
