//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009      Bastian Holst <bastianholst@gmx.de>
//

#ifndef SCREENGRAPHICSITEMPRIVATE_H
#define SCREENGRAPHICSITEMPRIVATE_H

#include "MarbleGraphicsItem_p.h"

namespace Marble {

class GeoGraphicsItemPrivate : public MarbleGraphicsItemPrivate {
 public:
    GeoGraphicsItemPrivate()
        : MarbleGraphicsItemPrivate(),
          m_positions()
    {
    }
    
    virtual ~GeoGraphicsItemPrivate() {
    }
     
    QList<QPoint> positions() {
        QList<QPoint> list;
        
        list.append( m_positions );
        return list;
    }
    
    void setProjection( AbstractProjection *projection, ViewportParams *viewport ) {
        m_positions.clear();
        
        qreal x[100], y;
        int pointRepeatNumber;
        bool globeHidesPoint;
        
        if( projection->screenCoordinates( m_coordinate,
                                           viewport,
                                           x, y,
                                           pointRepeatNumber,
                                           m_size,
                                           globeHidesPoint ) )
        {
            for( int i = 0; i < pointRepeatNumber; ++i ) {
                qint32 leftX = x[i] - ( m_size.width() / 2 );
                qint32 topY = y    - ( m_size.height() / 2 );
            
                m_positions.append( QPoint( leftX, topY ) );
            }
        }
    }
    
    GeoDataCoordinates m_coordinate;
    QString m_target;
    QList<QPoint> m_positions;
};

}

#endif // SCREENGRAPHICSITEMPRIVATE_H
