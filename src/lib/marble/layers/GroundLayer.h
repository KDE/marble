// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2012 Cezar Mocan <mocancezar@gmail.com>
//

#ifndef MARBLE_GROUNDLAYER_H
#define MARBLE_GROUNDLAYER_H

#include "LayerInterface.h"

#include <QColor>
#include <QPainterPath>

namespace Marble
{

class GroundLayer : public LayerInterface
{
public:
    GroundLayer();

    ~GroundLayer() override;

    QStringList renderPosition() const override;

    bool render(GeoPainter *painter, ViewportParams *viewport, const QString &renderPos = QLatin1String("NONE"), GeoSceneLayer *layer = nullptr) override;

    qreal zValue() const override;

    void setColor(const QColor &color);

    QColor color() const;

    RenderState renderState() const override;

    QString runtimeTrace() const override
    {
        return QStringLiteral("GroundLayer");
    }

private:
    QColor m_color; // Gets the color specified via DGML's <map bgcolor="">
};

}

#endif
