//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007      Andrew Manson  <g.real.ate@gmail.com>
//
#ifndef ABSTRACTLAYERINTERFACE_H
#define ABSTRACTLAYERINTERFACE_H

class ClipPainter;
class QPoint;
class QSize;
class Quaternion;
class BoundingBox;

class AbstractLayerInterface
{
  public:
    virtual void draw ( ClipPainter *painter, const QPoint &point )=0;
    
    virtual void draw( ClipPainter *painter, 
                       const QSize &canvasSize, double radius,
                       Quaternion invRotAxis )=0;
    
    virtual void draw( ClipPainter *painter, 
                       const QSize &canvasSize, double radius,
                       Quaternion invRotAxis, BoundingBox box )=0;
    
    bool visible() const;
    void setVisible( bool visible );
 
 protected:
    bool m_visible;
};

#endif
