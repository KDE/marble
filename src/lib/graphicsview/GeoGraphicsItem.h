//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009      Bastian Holst <bastianholst@gmx.de>
// Copyright 2009      Andrew Manson <g.real.ate@gmail.com>
//

#ifndef MARBLE_GEOGRAPHICSITEM_H
#define MARBLE_GEOGRAPHICSITEM_H

// Marble
#include "marble_export.h"

class QString;

namespace Marble
{

class GeoDataCoordinates;
class GeoDataLatLonAltBox;

class GeoGraphicsItemPrivate;
class GeoDataStyle;
class GeoPainter;
class GeoSceneLayer;
class ViewportParams;

class MARBLE_EXPORT GeoGraphicsItem
{
 public:
    GeoGraphicsItem();
    virtual ~GeoGraphicsItem();

    enum GeoGraphicsItemFlag {
        NoOptions = 0x0,
        ItemIsMovable = 0x1,
        ItemIsSelectable = 0x2,
        ItemIsVisible = 0x4
    };

    Q_DECLARE_FLAGS(GeoGraphicsItemFlags, GeoGraphicsItemFlag)

    bool visible() const;

    void setVisible( bool visible );

    /**
     * Return the coordinate of the item as a GeoDataCoordinates
     */
    virtual GeoDataCoordinates coordinate() const;

    /**
     * Set the coordinate of the item with an @p GeoDataPoint.
     */
    void setCoordinate( const GeoDataCoordinates &point );

    /**
     * Get the GeoGraphicItemFlags value that describes which flags are set on
     * this item. @see QFlags
     */
    GeoGraphicsItemFlags flags() const;

    /**
     * Set or unset a single flag
     * @param enabled sets if the flag is to be set or unset
     */
    void setFlag( GeoGraphicsItemFlag flag, bool enabled = true );

    /**
     * Replace all of the current flags.
     * @param flags is the new value for this item's flags.
     */
    void setFlags( GeoGraphicsItemFlags flags );

    /**
     * Returns the minim zoom level on which item will be active.
     */
    int minZoomLevel() const;

    /**
     * Sets the minimum zoom level
     */
    void setMinZoomLevel( int zoomLevel );

    /**
     * Returns the box that is used to determine if an item is active or inactive.
     */
    virtual GeoDataLatLonAltBox& latLonAltBox() const;

    /**
     * Set the box used to determine if an item is active or inactive. If an empty box is passed
     * the item will be shown in every case.
     */
    void setLatLonAltBox( const GeoDataLatLonAltBox& latLonAltBox );
    
    /**
     * Returns the style of item.
     */
    const GeoDataStyle* style() const;

    /**
     * Set the box used to determine if an item is active or inactive. If an empty box is passed
     * the item will be shown in every case.
     */
    void setStyle( const GeoDataStyle* style );

    /**
     * Returns the z value of the item
     */
    qreal zValue() const;

    /**
     * Set the z value of the item
     */
    void setZValue( qreal z );

    /**
     * Paints the item using the given GeoPainter.
     *
     * Note that depending on the projection and zoom level, the item may be visible more than once.
     * GeoPainter will therefore automatically "repeat" primitives which have a geo position (GeoDataCoordinates).
     */
    virtual void paint( GeoPainter *painter, ViewportParams *viewport,
                        const QString& renderPos, GeoSceneLayer * layer = 0 ) = 0;

 protected:
    GeoGraphicsItemPrivate *p() const;

 private:
    GeoGraphicsItemPrivate *const d;
};

} // Namespace Marble
Q_DECLARE_OPERATORS_FOR_FLAGS(Marble::GeoGraphicsItem::GeoGraphicsItemFlags)

#endif
