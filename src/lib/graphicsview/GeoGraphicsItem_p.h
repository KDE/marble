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


// Marble
#include "MarbleGraphicsItem_p.h"
#include "GeoDataLatLonAltBox.h"

namespace Marble
{

class GeoGraphicsItemPrivate : public MarbleGraphicsItemPrivate
{
 public:
    explicit GeoGraphicsItemPrivate( GeoGraphicsItem *parent )
        : MarbleGraphicsItemPrivate( parent ),
          m_positions(),
          m_minLodPixels( 0 ),
          m_maxLodPixels( -1 ),
          m_latLonAltBox()
    {
    }
    
    virtual ~GeoGraphicsItemPrivate()
    {
    }

    QList<QPointF> positions() const
    {
        return m_positions;
    }

    void setProjection( AbstractProjection *projection, ViewportParams *viewport )
    {
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
    QList<QPointF> m_positions;
    GeoGraphicsItem::GeoGraphicsItemFlags m_flags;

    // LOD
    int m_minLodPixels;
    int m_maxLodPixels;
    GeoDataLatLonAltBox m_latLonAltBox;
};

}

#endif // SCREENGRAPHICSITEMPRIVATE_H
