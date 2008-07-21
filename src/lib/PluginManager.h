//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2006-2008 Torsten Rahn <tackat@kde.org>"
//


#ifndef PLUGINMANAGER_H
#define PLUGINMANAGER_H

#include <QtCore/QObject>
#include "marble_export.h"

class MarbleAbstractLayer;
class MarbleAbstractFloatItem;
class PluginManagerPrivate;

/**
 * @short The class that handles Marble's plugins.
 *
 */

class MARBLE_EXPORT PluginManager : public QObject
{
    Q_OBJECT

 public:
    explicit PluginManager( QObject *parent = 0 );
    ~PluginManager();

    QList<MarbleAbstractFloatItem *> floatItems() const;
    QList<MarbleAbstractLayer *> layerPlugins() const;

 public Q_SLOTS:
    /**
     * @brief Browses the plugin directories and installs plugins. 
     *
     * This method browses all plugin directories and installs all  
     * plugins found in there.
     */
    void loadPlugins();

 private:
    Q_DISABLE_COPY( PluginManager )

    PluginManagerPrivate  * const d;
};


#endif // PLUGINMANAGER_H
