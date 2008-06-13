//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2008 Torsten Rahn <tackat@kde.org>"
//


// Own
#include "PluginManager.h"

// Qt
#include <QtCore/QDebug>
#include <QtCore/QPluginLoader>

// Local dir
#include "MarbleDirs.h"
#include "MarbleLayerInterface.h"

PluginManager::PluginManager(QObject *parent)
    : QObject(parent)
{
    // For testing:
    loadPlugins();
}

PluginManager::~PluginManager()
{
}

QList<MarbleLayerInterface *> PluginManager::layerInterfaces() const
{
    return m_layerInterfaces;
}

void PluginManager::loadPlugins()
{
    qDebug() << "Starting to load Plugins.";
    QStringList pluginFileNameList;

    pluginFileNameList = MarbleDirs::pluginEntryList( "", QDir::Files );

    MarbleDirs::debug();

    qDeleteAll( m_layerInterfaces );
    m_layerInterfaces.clear();

    foreach( QString fileName, pluginFileNameList ) {
        qDebug() << fileName << " - " << MarbleDirs::pluginPath( fileName );
        QPluginLoader loader( MarbleDirs::pluginPath( fileName ) );

        QObject *interface = loader.instance();

        if( interface ) {
            m_layerInterfaces.append( qobject_cast<MarbleLayerInterface *>(interface) );
        }
        else
        {
            qDebug() << "Plugin Failure: " << fileName << " is not a valid Marble Plugin:";
            qDebug() << loader.errorString();
        }
    }
}

#include "PluginManager.moc"
