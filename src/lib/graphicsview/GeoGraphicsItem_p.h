//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009      Bastian Holst <bastianholst@gmx.de>
//

#ifndef MARBLE_GEOGRAPHICSITEMPRIVATE_H
#define MARBLE_GEOGRAPHICSITEMPRIVATE_H


// Marble
#include "MarbleGraphicsItem_p.h"
#include "GeoDataLatLonAltBox.h"
#include "GeoDataStyle.h"
#include "ViewportParams.h"

namespace Marble
{

class GeoGraphicsItemPrivate : public MarbleGraphicsItemPrivate
{
 public:
    explicit GeoGraphicsItemPrivate( GeoGraphicsItem *parent )
        : MarbleGraphicsItemPrivate( parent ),
          m_zValue( 0 ),
          m_positions(),
          m_minZoomLevel( 0 ),
          m_latLonAltBox(),
          m_style( 0 )
    {
    }
    
    virtual ~GeoGraphicsItemPrivate()
    {
    }

    QList<QPointF> positions() const
    {
        return m_positions;
    }

    QList<QPointF> absolutePositions() const
    {
        return m_positions;
    }

    void setProjection( ViewportParams *viewport )
    {
        m_positions.clear();

        qreal x[100], y;
        int pointRepeatNumber;
        bool globeHidesPoint;

        if( viewport->screenCoordinates( m_coordinate,
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
    qreal m_zValue;
    QList<QPointF> m_positions;
    GeoGraphicsItem::GeoGraphicsItemFlags m_flags;

    int m_minZoomLevel;
    
    GeoDataLatLonAltBox m_latLonAltBox;
    const GeoDataStyle *m_style;
};

}

#endif
