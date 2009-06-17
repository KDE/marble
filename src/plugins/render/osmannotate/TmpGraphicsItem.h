//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009      Andrew Manson <g.real.ate@gmail.com>
//

#ifndef TMPGRAPHICSITEM_H
#define TMPGRAPHICSITEM_H

#include <QtCore/QRect>
#include <QtCore/QObject>
#include <QtGui/QPainterPath>

#include "GeoGraphicsItem.h"
#include "GeoDataLatLonAltBox.h"
#include "marble_export.h"

namespace Marble {

class MARBLE_EXPORT TmpGraphicsItem : public GeoGraphicsItem
{
public:
    TmpGraphicsItem();
    ~TmpGraphicsItem();


    virtual QRect screenBounding() = 0 ;
    virtual void geoBounding(qreal angularResolution) = 0;

    virtual void paint(GeoPainter * painter, ViewportParams * viewport, const QString &renderPos, GeoSceneLayer *layer) =0 ;

    //this is just so that i can test the implementation
    QList<QPainterPath>  m_regions;




};

}

#endif // TMPGRAPHICSITEM_H
