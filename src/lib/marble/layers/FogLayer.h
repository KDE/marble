// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2006-2007 Torsten Rahn <tackat@kde.org>
// SPDX-FileCopyrightText: 2007 Inge Wallin <ingwa@kde.org>
// SPDX-FileCopyrightText: 2008, 2009, 2010 Jens-Michael Hoffmann <jmho@c-xx.com>
// SPDX-FileCopyrightText: 2008-2009 Patrick Spendrin <ps_ml@gmx.de>
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

    bool render(GeoPainter *painter, ViewportParams *viewport, const QString &renderPos = QLatin1String("NONE"), GeoSceneLayer *layer = nullptr) override;

    RenderState renderState() const override;
    QString runtimeTrace() const override
    {
        return QStringLiteral("FogLayer");
    }
};

}

#endif
