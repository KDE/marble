//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011 Guillaume Martres <smarter@ubuntu.com>
//

#ifndef MARBLE_TRACKERPLUGINMODEL_H
#define MARBLE_TRACKERPLUGINMODEL_H

#include "marble_export.h"

#include <QtCore/QObject>

class QUrl;

namespace Marble
{

class GeoDataTreeModel;
class PluginManager;
class TrackerPluginItem;
class TrackerPluginModelPrivate;

class MARBLE_EXPORT TrackerPluginModel : public QObject
{
    Q_OBJECT
public:
    TrackerPluginModel( GeoDataTreeModel *treeModel, const PluginManager *pluginManager  );

    TrackerPluginItem *item( const QString &name );
    QList<TrackerPluginItem *> items();
    void addItem( TrackerPluginItem *mark );
    bool removeItem( const QString &name );

    void beginUpdatePlacemarks();
    void endUpdatePlacemarks();

    void downloadFile( const QUrl &url, const QString &id );
    virtual void parseFile( const QString &id, const QByteArray &file );

private:
    TrackerPluginModelPrivate *d;
    Q_PRIVATE_SLOT( d, void downloaded( const QString &, const QString & ) );
    Q_PRIVATE_SLOT( d, void update() );
};

}

#endif // MARBLE_TRACKERPLUGINMODEL_H
