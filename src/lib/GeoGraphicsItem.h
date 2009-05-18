//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009      Bastian Holst <bastianholst@gmx.de>
//

#ifndef GEOGRAPHICSITEM_H
#define GEOGRAPHICSITEM_H

// Marble
#include "MarbleGraphicsItem.h"
#include "marble_export.h"

// Qt
#include <QtCore/QList>

class QPoint;
class QRect;
class QString;

namespace Marble {
    
class GeoDataCoordinates;
class GeoDataPoint;
class GeoPainter;
class GeoSceneLayer;
class ViewportParams;

class GeoGraphicsItemPrivate;

class MARBLE_EXPORT GeoGraphicsItem : public MarbleGraphicsItem {
 public:
    GeoGraphicsItem();
    GeoGraphicsItem( GeoGraphicsItemPrivate *d_ptr );
    virtual ~GeoGraphicsItem();
    
    /**
     * Return the coordinate of the item as a GeoDataCoordinates
     */
    GeoDataCoordinates coordinate() const;
                        
    /**
     * Return the coordinate of the item as @p longitude
     * and @p latitude.
     */
    void coordinate( qreal &longitude, qreal &latitude, qreal &altitude ) const;

    /**
     * Set the coordinate of the item in @p longitude and
     * @p latitude.
     */
    void setCoordinate( qreal longitude, qreal latitude, qreal altitude = 0 );
    
    /**
     * Set the coordinate of the item with an @p GeoDataPoint.
     */
    void setCoordinate( const GeoDataPoint &point );
    
    /**
     * Get the target of the item. The target is the current planet string.s
     */
    QString target();
    
    /**
     * Set the target of the item with @p target.
     */
    void setTarget( const QString& target );
    
    /**
     * Returns all coordinates of the item in view coordinates according to the given projection.
     */
    QList<QPoint> positions();

 protected:
    /**
     * Paints the item in item coordinates
     */
    virtual bool paint( GeoPainter *painter, ViewportParams *viewport,
                        const QString& renderPos, GeoSceneLayer * layer = 0 ) = 0;
    
 private:     
    GeoGraphicsItemPrivate *p() const;
};

} // Namespace Marble

#endif // GEOGRAPHICSITEM_H
