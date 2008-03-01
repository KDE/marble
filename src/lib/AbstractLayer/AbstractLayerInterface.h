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

#include <QtCore/QTextStream>

#include "ViewParams.h"

class ClipPainter;
class QPoint;
class QSize;
class BoundingBox;

class AbstractLayerInterface
{
    friend QTextStream& operator<<( QTextStream&, 
                                    const AbstractLayerInterface & );
  public:
    virtual ~AbstractLayerInterface();

    virtual void draw ( ClipPainter *painter, const QPoint &point )=0;
    
    virtual void draw( ClipPainter *painter, 
                       const QSize &canvasSize, 
                       ViewParams *viewParams )=0;
    
    virtual void draw( ClipPainter *painter, 
                       const QSize &canvasSize, ViewParams *viewParams,
                       BoundingBox &box )=0;
    
    virtual void printToStream( QTextStream& ) const;
    
    bool visible() const;
    void setVisible( bool visible );
 
 protected:
    bool m_visible;
};

#endif
