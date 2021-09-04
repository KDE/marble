// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2011 Guillaume Martres <smarter@ubuntu.com>
//

#ifndef MARBLE_TRACKERPLUGINITEM_H
#define MARBLE_TRACKERPLUGINITEM_H

#include <qglobal.h>

class QString;

namespace Marble {

class GeoDataPlacemark;
class TrackerPluginItemPrivate;

/**
 * Subclass this to represent items in your TrackerPluginModel.
 */
class TrackerPluginItem
{
public:
    /**
     * Constructs an item and set the wrapped placemark name to @p name
     */
    explicit TrackerPluginItem( const QString &name );

    /**
     * Destroy the item.
     */
    virtual ~TrackerPluginItem();

    /**
     * Satellite's name
     */
    QString name() const;

    /**
     * Returns the wrapped placemark which will be displayed if this item is in a TrackerPluginModel
     */
    GeoDataPlacemark *placemark();

    /**
     * Returns whether the item is enabled or disabled.
     */
    virtual bool isEnabled() const;

    /**
     * Enable/Disable the item following the user checkbox action according to @p enabled.
     */
    virtual void setEnabled( bool enabled );

    /**
     * Return whether the item is visible or invisible.
     */
    virtual bool isVisible() const;

    /**
     * Set item visible/invisible according to @p visible.
     */
    virtual void setVisible( bool visible );

    /**
     * Return whether the track is visible or invisible.
     */
    virtual bool isTrackVisible() const;

    /**
     * Set item track visible/invisible according to @p visible.
     */
    virtual void setTrackVisible( bool visible );

    /**
     * Reimplement this method to update the placemark, for example to change its coordinates.
     * If this item is in a TrackerPluginModel, this method will be called regularly.
     */
    virtual void update() = 0;

private:
    Q_DISABLE_COPY(TrackerPluginItem)
    TrackerPluginItemPrivate *d;
};

} // namespace Marble

#endif // MARBLE_TRACKERPLUGINITEM_H
