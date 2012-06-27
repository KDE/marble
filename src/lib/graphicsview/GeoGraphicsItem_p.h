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
          m_minLodPixels( 0 ),
          m_maxLodPixels( -1 ),
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

    void setProjection( ViewportParams *viewport,
                        GeoPainter *painter )
    {
        m_positions.clear();

        if ( !isActive( painter ) ) {
            return;
        }

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

    /**
     * Returns true if the item is active and should be shown.
     * This depends on the LOD settings.
     */
    virtual bool isActive( GeoPainter *painter )
    {
        if ( m_latLonAltBox.isNull()
             || ( m_minLodPixels == 0 && m_maxLodPixels == -1 ) )
        {
            return true;
        }

        QRegion region = painter->regionFromRect( m_latLonAltBox.center(),
                                                  m_latLonAltBox.width(),
                                                  m_latLonAltBox.height(),
                                                  true );

        int pixels = 0;

        foreach( QRect rect, region.rects() ) {
            pixels += rect.width() * rect.height();
        }

        if ( pixels >= m_minLodPixels
             && pixels <= m_maxLodPixels )
        {
            return true;
        }
        else {
            return false;
        }
    }

    GeoDataCoordinates m_coordinate;
    QString m_target;
    qreal m_zValue;
    QList<QPointF> m_positions;
    GeoGraphicsItem::GeoGraphicsItemFlags m_flags;

    // LOD
    int m_minLodPixels;
    int m_maxLodPixels;
    
    int m_minZoomLevel;
    
    GeoDataLatLonAltBox m_latLonAltBox;
    GeoDataStyle *m_style;
};

}

#endif
