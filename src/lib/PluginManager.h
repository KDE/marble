//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2006-2008 Torsten Rahn <tackat@kde.org>
// Copyright 2009      Jens-Michael Hoffmann <jensmh@gmx.de>
//

#ifndef MARBLE_PLUGINMANAGER_H
#define MARBLE_PLUGINMANAGER_H

#include <QtCore/QList>
#include "marble_export.h"


namespace Marble
{

class RenderPlugin;
class NetworkPlugin;
class PositionProviderPlugin;
class AbstractFloatItem;
class PluginManagerPrivate;
class SearchRunnerPlugin;
class ReverseGeocodingRunnerPlugin;
class RoutingRunnerPlugin;
class ParseRunnerPlugin;

/**
 * @short The class that handles Marble's plugins.
 *
 * Ownership policy for plugins:
 *
 * On every invocation of createNetworkPlugins and
 * createFloatItems the PluginManager creates new objects and transfers
 * ownership to the calling site. In order to create
 * the objects, the PluginManager internally has a list of the plugins
 * which are owned by the PluginManager and destroyed by it.
 *
 */

class MARBLE_EXPORT PluginManager
{
 public:
    explicit PluginManager();

    ~PluginManager();

    /**
     * @brief Returns all available RenderPlugins.
     *
     * Ownership of the items remains in PluginManager.
     * In order to use the RenderPlugins, first create new instances using
     * RenderPlugin::newInstance().
     */
    QList<const RenderPlugin *> renderPlugins() const;

    /**
     * @brief Returns all available NetworkPlugins.
     *
     * Ownership of the items remains in PluginManager.
     * In order to use the NetworkPlugins, first create new instances using
     * NetworkPlugin::newInstance().
     */
    QList<const NetworkPlugin *> networkPlugins() const;

    /**
     * @brief Returns all available PositionProviderPlugins.
     *
     * Ownership of the items remains in PluginManager.
     * In order to use the PositionProviderPlugins, first create new instances using
     * PositionProviderPlugin::newInstance().
     */
    QList<const PositionProviderPlugin *> positionProviderPlugins() const;

    /**
     * Returns all search runner plugins.
     * @note: Runner plugins are owned by the PluginManager, do not delete them.
     */
    QList<const SearchRunnerPlugin *> searchRunnerPlugins() const;

    /**
     * Returns all reverse geocoding runner plugins.
     * @note: The runner plugins are owned by the PluginManager, do not delete them.
     */
    QList<const ReverseGeocodingRunnerPlugin *> reverseGeocodingRunnerPlugins() const;

    /**
     * Returns all routing runner plugins.
     * @note: The runner plugins are owned by the PluginManager, do not delete them.
     */
    QList<RoutingRunnerPlugin *> routingRunnerPlugins() const;

    /**
     * Returns all parse runner plugins.
     * @note: The runner plugins are owned by the PluginManager, do not delete them.
     */
    QList<const ParseRunnerPlugin *> parsingRunnerPlugins() const;

 private:
    Q_DISABLE_COPY( PluginManager )

    PluginManagerPrivate  * const d;
};

}

#endif
