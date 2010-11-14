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

enum GeoGraphicsItemChange {
    ItemPositionChange,
    ItemSelectChange
};

    //method will be removed when we decide how to do the general case of
    //a hit test. Waiting for tackat to do "The Maths"
//    virtual QRect screenBounding() = 0 ;
    // same as above
//    virtual void geoBounding(qreal angularResolution) = 0;

    virtual void paint(GeoPainter * painter, ViewportParams * viewport, const QString &renderPos, GeoSceneLayer *layer) =0 ;

    //Designed to recreated the QGraphicsItem::itemChange()
    virtual QVariant itemChange( GeoGraphicsItemChange change, QVariant Value );

    QList<QRegion> regions() const;


    QList<TmpGraphicsItem*> children() const;
    void addChild(TmpGraphicsItem* c);

    TmpGraphicsItem* parent();
    void setParent( TmpGraphicsItem* p );

    //Start dealing with the event stuff
    //is this the best thing to call the event distributer?
    bool sceneEvent( QEvent* event );

protected:
    //FIXME remove this? is this needed at all?
    void setGeoOffset( bool g );

    void setRegions( const QList<QRegion>& regions );


    virtual bool mousePressEvent( QMouseEvent* event );

 private:
    //intended to go in the private class
    QList<TmpGraphicsItem*> m_children;

    QList<QRegion> m_regions;

    //Allows for the implementation of local coordinate systems
    //for children objects
    TmpGraphicsItem* m_parent;

    bool geoOffset;




};

}

#endif // TMPGRAPHICSITEM_H
