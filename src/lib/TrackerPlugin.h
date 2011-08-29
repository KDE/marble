//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011 Guillaume Martres <smarter@ubuntu.com>
//

#ifndef MARBLE_TRACKERPLUGIN_H
#define MARBLE_TRACKERPLUGIN_H

#include "RenderPlugin.h"

namespace Marble {

class GeoDataPlacemark;
class TrackerPluginItem;
class TrackerPluginPrivate;

/**
 * A RenderPlugin designed to track objects as placemarks
 */
class MARBLE_EXPORT TrackerPlugin : public RenderPlugin
{
    Q_OBJECT
public:
    TrackerPlugin();

    virtual void initialize();

    virtual bool render( GeoPainter *painter, ViewportParams *viewport, const QString &renderPos, GeoSceneLayer *layer );

    TrackerPluginItem *item( const QString &name );
    QList<TrackerPluginItem *> items();
    void addItem( TrackerPluginItem *mark );
    bool removeItem( const QString &name );

    void beginUpdatePlacemarks();
    void endUpdatePlacemarks();

    void downloadFile( const QUrl &url, const QString &id );
    virtual void parseFile( const QString &id, const QByteArray &file );

public Q_SLOTS:
    virtual void update();

private:
    TrackerPluginPrivate *d;
    Q_PRIVATE_SLOT( d, void downloaded( const QString &, const QString & ) );
};

}

#endif // MARBLE_TRACKERPLUGIN_H
