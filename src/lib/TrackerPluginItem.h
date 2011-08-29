//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011 Guillaume Martres <smarter@ubuntu.com>
//

#ifndef MARBLE_TRACKERPLUGINITEM_H
#define MARBLE_TRACKERPLUGINITEM_H

#include "marble_export.h"

class QString;

namespace Marble {

class GeoDataPlacemark;
class GeoPainter;
class GeoSceneLayer;
class TrackerPluginItemPrivate;
class ViewportParams;

class MARBLE_EXPORT TrackerPluginItem
{
public:
    TrackerPluginItem( const QString &name );
    virtual ~TrackerPluginItem();

    GeoDataPlacemark *placemark();

    virtual void render( GeoPainter *painter, ViewportParams *viewport, const QString &renderPos, GeoSceneLayer *layer );

    virtual void update() = 0;

private:
    TrackerPluginItemPrivate *d;
};

}

#endif // MARBLE_TRACKERPLUGINITEM_H
