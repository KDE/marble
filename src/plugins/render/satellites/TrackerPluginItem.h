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

#include <QtCore/QObject>

class QString;

namespace Marble {

class GeoDataPlacemark;
class GeoPainter;
class GeoSceneLayer;
class TrackerPluginItemPrivate;
class ViewportParams;

/**
 * Subclass this to represent items in your TrackerPluginModel.
 */
class TrackerPluginItem : public QObject
{
    Q_OBJECT

public:
    /**
     * Constructs an item and set the wrapped placemark name to @p name
     */
    TrackerPluginItem( const QString &name );

    /**
     * Destroy the item.
     */
    virtual ~TrackerPluginItem();

    /**
     * Returns the wrapped placemark which will be displayed if this item is in a TrackerPluginModel
     */
    GeoDataPlacemark *placemark();

    /**
     * Reimplement this method to update the placemark, for example to change its coordinates.
     * If this item is in a TrackerPluginModel, this method will be called regularly.
     */
    virtual void update() = 0;

private:
    TrackerPluginItemPrivate *d;
};

} // namespace Marble

#endif // MARBLE_TRACKERPLUGINITEM_H
