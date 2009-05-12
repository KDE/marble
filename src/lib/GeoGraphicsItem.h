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

class GeoGraphicsItem : public MarbleGraphicsItem {
 public:
    GeoGraphicsItem();
    ~GeoGraphicsItem();
    
    /**
     * Paints the item
     */
    virtual bool paint( GeoPainter *painter, ViewportParams *viewport,
                        const QString& renderPos, GeoSceneLayer * layer = 0 ) = 0;
    
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
     * Get the target of the item. The target is the current planet string
     */
    QString target();
    
    /**
     * Set the target of the item with @p target.
     */
    void setTarget( QString target );

 private:
    GeoGraphicsItemPrivate *d;
};

#endif // GEOGRAPHICSITEM_H