//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009      Bastian Holst <bastianholst@gmx.de>
//

#ifndef MARBLEGRAPHICSITEMPRIVATE_H
#define MARBLEGRAPHICSITEMPRIVATE_H

// Marble
#include "AbstractProjection.h"

// Qt
#include<QtCore/QList>
#include<QtCore/QSize>
#include<QtCore/QSizeF>
#include<QtCore/QRect>
#include<QtGui/QPixmapCache>

namespace Marble {

class MarbleGraphicsItemPrivate {
 public:
    MarbleGraphicsItemPrivate()
        : m_removeCachedPixmap( false ),
          m_cacheMode( MarbleGraphicsItem::NoCache )
    {
    }
    
    virtual ~MarbleGraphicsItemPrivate() {
    }
     
    virtual QList<QPointF> positions() {
        return QList<QPointF>();
    }
    
    void ensureValidCacheKey() {
        if( m_cacheKey.isNull() ) {
            static unsigned int key = 0;
            m_cacheKey = QString( "MarbleGraphicsItem:" ) + QString::number( key );
            key++;
        }
    }
    
    QList<QRectF> boundingRects() {
        QList<QRectF> list;
        
        foreach( QPointF point, positions() ) {
            QRectF rect( point, m_size );
            if( rect.x() < 0 )
                rect.setLeft( 0 );
            if( rect.y() < 0 )
                rect.setTop( 0 );
            
            list.append( rect );
        }
        
        return list;
    }
    
    virtual void setProjection( AbstractProjection *projection, ViewportParams *viewport ) {
        Q_UNUSED( projection );
        Q_UNUSED( viewport );
    };
    
    QSizeF m_size;
    QSize m_logicalCacheSize;
    
    bool m_removeCachedPixmap;
    
    MarbleGraphicsItem::CacheMode m_cacheMode;
    
    // TODO: Substitute this by QPixmapCache::Key once it is available.
    QString m_cacheKey;
};

}

#endif // MARBLEGRAPHICSITEMPRIVATE_H
