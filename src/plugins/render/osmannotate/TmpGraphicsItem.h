//
// This file is part of the Marble Virtual Globe.
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
#include <QtGui/QMouseEvent>

#include "GeoGraphicsItem.h"
#include "GeoDataLatLonAltBox.h"
#include "marble_export.h"

namespace Marble
{



class TmpGraphicsItem : public GeoGraphicsItem
{
public:
    TmpGraphicsItem();
    ~TmpGraphicsItem();

    virtual void paint(GeoPainter * painter, const ViewportParams * viewport) =0;

    QList<QRegion> regions() const;

    //Start dealing with the event stuff
    //is this the best thing to call the event distributer?
    bool sceneEvent( QEvent* event );

protected:
    void setRegions( const QList<QRegion>& regions );


    virtual bool mousePressEvent( QMouseEvent* event );

 private:
    QList<QRegion> m_regions;

};

}

#endif // TMPGRAPHICSITEM_H
