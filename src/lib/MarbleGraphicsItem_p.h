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
#include<QtCore/QRect>
#include<QtGui/QPixmapCache>

namespace Marble {

class MarbleGraphicsItemPrivate {
 public:
    MarbleGraphicsItemPrivate()
        : m_cacheMode( MarbleGraphicsItem::NoCache )
    {
    }
    
    virtual ~MarbleGraphicsItemPrivate() {
        QPixmapCache::remove( m_cacheKey );
    }
     
    virtual QList<QPoint> positions() {
        return QList<QPoint>();
    }
    
    void ensureValidCacheKey() {
        if( m_cacheKey.isNull() ) {
            static unsigned int key = 0;
            m_cacheKey = QString( "MarbleGraphicsItem:" ) + QString::number( key );
            key++;
        }
    }
    
    QList<QRect> boundingRects() {
        QList<QRect> list;
        
        foreach( QPoint point, positions() ) {
            QRect rect( point, m_size );
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
    
    QSize m_size;
    QSize m_logicalCacheSize;
    
    MarbleGraphicsItem::CacheMode m_cacheMode;
    
    // TODO: Substitute this by QPixmapCache::Key once it is available.
    QString m_cacheKey;
};

}

#endif // MARBLEGRAPHICSITEMPRIVATE_H
