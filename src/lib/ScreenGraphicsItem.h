//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009      Bastian Holst <bastianholst@gmx.de>
//

#ifndef SCREENGRAPHICSITEM_H
#define SCREENGRAPHICSITEM_H

// Marble
#include "MarbleGraphicsItem.h"
#include "marble_export.h"

// Qt
#include <QtCore/QList>

class QPoint;
class QRect;
class QString;

namespace Marble {

class GeoPainter;
class ViewportParams;

class ScreenGraphicsItemPrivate;

class MARBLE_EXPORT ScreenGraphicsItem : public MarbleGraphicsItem {
 public:
    ScreenGraphicsItem();
    ScreenGraphicsItem( ScreenGraphicsItemPrivate *d_ptr );
    
    virtual ~ScreenGraphicsItem();
    
    /**
     * Returns the postion of the item
     */
    QPoint position() const;
    
    /**
     * Set the position of the item
     */
    void setPosition( const QPoint& position );
    
 protected:
    /**
     * Paints the item in item coordinates
     */
    virtual bool paint( GeoPainter *painter, ViewportParams *viewport,
                        const QString& renderPos, GeoSceneLayer * layer = 0 ) = 0;
     
 private:
    ScreenGraphicsItemPrivate *p() const;
};

} // Namespace Marble

#endif // GEOGRAPHICSITEM_H
