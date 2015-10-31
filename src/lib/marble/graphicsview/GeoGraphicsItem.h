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
#include <GeoDataStyle.h>

class QString;

namespace Marble
{

class GeoDataFeature;
class GeoDataLatLonAltBox;
class GeoGraphicsItemPrivate;
class GeoPainter;
class ViewportParams;

class MARBLE_EXPORT GeoGraphicsItem
{
 public:
    explicit GeoGraphicsItem( const GeoDataFeature *feature );
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
     * Returns the placemark for that item.
     */
    const GeoDataFeature* feature() const;

    /**
     * Returns the bounding box covered by the item.
     */
    virtual const GeoDataLatLonAltBox& latLonAltBox() const;

    /**
     * Set the box used to determine if an item is active or inactive. If an empty box is passed
     * the item will be shown in every case.
     */
    void setLatLonAltBox( const GeoDataLatLonAltBox& latLonAltBox );
    
    /**
     * Returns the style of item.
     */
    GeoDataStyle::ConstPtr style() const;

    /**
     * Set the style for the item.
     */
    void setStyle(const GeoDataStyle::ConstPtr &style );

    /**
     * Set the style which will be used when
     * placemark is highlighted.
     * GeoGraphicsItem takes ownership of the
     * passed style and deletes it when appropriate.
     */
    void setHighlightStyle( const GeoDataStyle::ConstPtr &highlightStyle );

    /**
     * Returns the z value of the item
     */
    qreal zValue() const;

    /**
     * Set the z value of the item
     */
    void setZValue( qreal z );

    static bool zValueLessThan(GeoGraphicsItem* one, GeoGraphicsItem* two);

    /**
     * Paints the item using the given GeoPainter.
     *
     * Note that depending on the projection and zoom level, the item may be visible more than once,
     * which is taken care of by GeoPainter.
     */
    virtual void paint( GeoPainter *painter, const ViewportParams *viewport ) = 0;

    void setHighlighted( bool highlight );

    bool isHighlighted() const;

    const QList<GeoGraphicsItem*>& decorations();

    bool isDecoration() const;

 protected:
    GeoGraphicsItemPrivate *const d;

    /**
     * Creates a new decoration for this item.
     *
     * Override this function to create a new type of decoration,
     * e.g. outlines for lines or "fake 3D effect" for polygons.
     * After a decoration was created add it to the item with
     * addDecoration(). You can create multiple decoration for a
     * single GeoGraphicsItem.
     */
    virtual void createDecorations();

    void addDecoration(GeoGraphicsItem* decoration);
};

} // Namespace Marble
Q_DECLARE_OPERATORS_FOR_FLAGS(Marble::GeoGraphicsItem::GeoGraphicsItemFlags)

#endif
