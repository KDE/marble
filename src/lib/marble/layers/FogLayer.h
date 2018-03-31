//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2006-2007 Torsten Rahn <tackat@kde.org>
// Copyright 2007      Inge Wallin  <ingwa@kde.org>
// Copyright 2008, 2009, 2010 Jens-Michael Hoffmann <jmho@c-xx.com>
// Copyright 2008-2009      Patrick Spendrin <ps_ml@gmx.de>
//

#ifndef MARBLE_FOGLAYER_H
#define MARBLE_FOGLAYER_H

#include "LayerInterface.h"

#include <QString>

namespace Marble
{

class FogLayer : public LayerInterface
{
public:
    QStringList renderPosition() const override;

    bool render( GeoPainter *painter, ViewportParams *viewport,
                         const QString& renderPos = QLatin1String("NONE"),
                         GeoSceneLayer * layer = nullptr ) override;

    RenderState renderState() const override;
    QString runtimeTrace() const override { return QStringLiteral("FogLayer"); }
};

}

#endif
