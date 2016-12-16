//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2012      Cezar Mocan <mocancezar@gmail.com>
//

#ifndef MARBLE_GROUNDLAYER_H
#define MARBLE_GROUNDLAYER_H

#include "LayerInterface.h"

#include <QColor>

namespace Marble
{

class GroundLayer : public LayerInterface
{
 public:

    GroundLayer();

    ~GroundLayer() override;

    QStringList renderPosition() const override;

    bool render( GeoPainter *painter, ViewportParams *viewport,
                         const QString& renderPos = QLatin1String("NONE"),
                         GeoSceneLayer * layer = 0 ) override;

    qreal zValue() const override;

    void setColor( const QColor &color );

    QColor color() const;

    RenderState renderState() const override;

    QString runtimeTrace() const override { return QStringLiteral("GroundLayer"); }

 private:
    QColor m_color;  // Gets the color specified via DGML's <map bgcolor="">
    
};

}

#endif
