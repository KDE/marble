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
    virtual GeoDataLatLonBox geoBounding(qreal angularResolution) = 0;
    //for this to work we MUST set the m_geoBoundCache after constructing it
    virtual GeoDataLatLonBox geoBounding();
    virtual void paint(GeoPainter * painter, ViewportParams * viewport, const QString &renderPos, GeoSceneLayer *layer) =0 ;


private:
    //so that its only made once?
    QRect m_screenBoundCache;
protected:
    //possibly cache the calculation of the geobound
    GeoDataLatLonBox m_geoBoundCache;

};

}

#endif // TMPGRAPHICSITEM_H
