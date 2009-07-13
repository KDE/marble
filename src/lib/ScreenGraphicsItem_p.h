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

class ScreenGraphicsItemPrivate : public MarbleGraphicsItemPrivate {
 public:
    ScreenGraphicsItemPrivate()
        : MarbleGraphicsItemPrivate(),
          m_position()
    {
    }
    
    virtual ~ScreenGraphicsItemPrivate() {
    }
     
    QList<QPointF> positions() {
        QList<QPointF> list;
        
        list.append( m_position );
        return list;
    }
    
    void setProjection( AbstractProjection *projection, ViewportParams *viewport ) {
        Q_UNUSED( projection );
        Q_UNUSED( viewport );
    }
    
    QPointF m_position;
};

}

#endif // SCREENGRAPHICSITEMPRIVATE_H
