//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011      Bernhard Beschow <bbeschow@cs.tu-berlin.de>
//

#ifndef MARBLE_FPSLAYER_H
#define MARBLE_FPSLAYER_H

#include "LayerInterface.h"

class QTime;

namespace Marble
{

class FpsLayer : public LayerInterface
{
public:
    explicit FpsLayer( QTime *time );

    /**
     * @reimp
     */
    virtual QStringList renderPosition() const;

    /**
     * @reimp
     */
    virtual bool render( GeoPainter *painter, ViewportParams *viewport,
       const QString& renderPos = "NONE", GeoSceneLayer * layer = 0 );

    /**
      * @reimp
      */
    virtual qreal zValue() const;

private:
    QTime *const m_time;
};

}

#endif
